use std::io::Write;
use std::path::PathBuf;
use std::process::Command;

/// CamelCase を snake_case に変換
fn camel_to_snake(s: &str) -> String {
    let mut result = String::new();
    for (i, c) in s.chars().enumerate() {
        if c.is_uppercase() {
            if i > 0 {
                result.push('_');
            }
            result.push(c.to_lowercase().next().unwrap());
        } else {
            result.push(c);
        }
    }
    result
}

fn main() {
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let build_dir = PathBuf::from(&out_dir).join("cpp_tests");

    std::fs::create_dir_all(&build_dir).unwrap();

    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let tests_dir = PathBuf::from(&manifest_dir).join("tests");

    // CMake configure
    let configure_status = Command::new("cmake")
        .args([tests_dir.to_str().unwrap(), "-DCMAKE_BUILD_TYPE=Debug"])
        .current_dir(&build_dir)
        .status()
        .expect("cmake configure failed");

    if !configure_status.success() {
        panic!("cmake configure failed with status: {}", configure_status);
    }

    // CMake build
    let build_status = Command::new("cmake")
        .args(["--build", ".", "-j"])
        .current_dir(&build_dir)
        .status()
        .expect("cmake build failed");

    if !build_status.success() {
        panic!("cmake build failed with status: {}", build_status);
    }

    // ctest -N でテスト一覧を取得
    let ctest_output = Command::new("ctest")
        .args(["-N"])
        .current_dir(&build_dir)
        .output()
        .expect("ctest -N failed");

    let test_list = String::from_utf8_lossy(&ctest_output.stdout);

    // テスト名を抽出（"Test  #1: TestName" or "Test #10: TestName" 形式）
    let test_names: Vec<String> = test_list
        .lines()
        .filter_map(|line| {
            // "Test" で始まり、"#" の後に数字と ":" がある行を探す
            if line.trim_start().starts_with("Test") {
                if let Some(colon_pos) = line.find(':') {
                    let name = line[colon_pos + 1..].trim();
                    if !name.is_empty() {
                        return Some(name.to_string());
                    }
                }
            }
            None
        })
        .collect();

    // テストコードを生成
    let mut generated_tests = String::new();
    generated_tests.push_str("// Auto-generated test wrappers for C++ GoogleTest tests\n\n");

    for test_name in &test_names {
        // Rust の識別子として有効な名前に変換（. を _ に）
        let rust_name = test_name.replace('.', "_").to_lowercase();

        // テストバイナリ名を推測
        // StreamRecBufferTest -> test_stream_rec_buffer
        let binary_name = if let Some(dot_pos) = test_name.find('.') {
            let suite = &test_name[..dot_pos];
            // "Test" サフィックスを除去
            let suite = suite.strip_suffix("Test").unwrap_or(suite);
            format!("test_{}", camel_to_snake(suite))
        } else {
            "test".to_string()
        };

        generated_tests.push_str(&format!(
            r#"#[test]
fn {rust_name}() {{
    let test_dir = env!("CPP_TEST_DIR");
    let binary = std::path::Path::new(test_dir).join("{binary_name}");
    let output = std::process::Command::new(&binary)
        .arg("--gtest_filter={test_name}")
        .output()
        .expect("failed to run test binary");

    if !output.status.success() {{
        eprintln!("{{}}", String::from_utf8_lossy(&output.stdout));
        eprintln!("{{}}", String::from_utf8_lossy(&output.stderr));
        panic!("C++ test {test_name} failed");
    }}
}}

"#,
            rust_name = rust_name,
            binary_name = binary_name,
            test_name = test_name
        ));
    }

    // 生成したコードをファイルに書き出し
    let generated_path = PathBuf::from(&out_dir).join("generated_tests.rs");
    let mut file = std::fs::File::create(&generated_path).expect("failed to create generated file");
    file.write_all(generated_tests.as_bytes())
        .expect("failed to write generated tests");

    // テストバイナリのパスを環境変数で渡す
    println!("cargo:rustc-env=CPP_TEST_DIR={}", build_dir.display());
    println!("cargo:rerun-if-changed=tests/");
}
