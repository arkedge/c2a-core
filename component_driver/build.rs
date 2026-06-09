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

/// `ctest -N` が出力するテスト名 1 件を、Rust テスト生成に必要な 3 要素に分解する。
///
/// GoogleTest の `DISABLED_` 付きテストは `gtest_discover_tests` によって
/// `DISABLED_` プレフィックスが落とされ、代わりに末尾へ ` (Disabled)` が付く。
///   例: `StreamRecBufferTest.InitWithZeroCapacity (Disabled)`
/// これをそのまま使うと、(1) Rust 識別子に空白・括弧が混ざりコンパイルエラーになる、
/// (2) `--gtest_filter` が実テスト名 (`DISABLED_` 付き) と一致しない、
/// という 2 つの問題が起きるため、ここで正規化する。
///
/// 戻り値 `(rust_ident, gtest_filter, ignored)`:
///   - `rust_ident`  : Rust の関数名として有効な小文字識別子
///                     (英数字以外はすべて '_' に変換する)
///   - `gtest_filter`: `--gtest_filter` にそのまま渡せる完全なテスト名。
///                     disabled の場合は `Suite.Name` → `Suite.DISABLED_Name` に復元する
///   - `ignored`     : disabled テストなら true (Rust 側で `#[ignore]` を付ける)
///
/// 期待する変換例:
///   "StreamRecBufferTest.PushDataSuccess"
///     -> ("streamrecbuffertest_pushdatasuccess",
///         "StreamRecBufferTest.PushDataSuccess", false)
///   "StreamRecBufferTest.InitWithZeroCapacity (Disabled)"
///     -> ("streamrecbuffertest_initwithzerocapacity",
///         "StreamRecBufferTest.DISABLED_InitWithZeroCapacity", true)
fn parse_test_entry(raw: &str) -> (String, String, bool) {
    let raw = raw.trim();

    // 1. 末尾の ` (Disabled)` 注釈で disabled を判定し、クリーンな "Suite.Name" を得る
    let (clean, ignored) = match raw.rfind(" (") {
        Some(pos) if raw[pos..].eq_ignore_ascii_case(" (disabled)") => (raw[..pos].trim(), true),
        _ => (raw, false),
    };

    // 2. disabled の場合は "Suite.Name" -> "Suite.DISABLED_Name" に復元する
    //    (gtest_discover_tests が落とした DISABLED_ を戻し、--gtest_filter にそのまま渡せる完全名にする)
    let gtest_filter = if ignored {
        match clean.rfind('.') {
            Some(dot) => format!("{}.DISABLED_{}", &clean[..dot], &clean[dot + 1..]),
            None => format!("DISABLED_{}", clean),
        }
    } else {
        clean.to_string()
    };

    // 3. クリーンな "Suite.Name" の英数字以外をすべて '_' に変換し、小文字化して Rust 識別子とする
    let rust_ident = clean
        .chars()
        .map(|c| {
            if c.is_ascii_alphanumeric() {
                c.to_ascii_lowercase()
            } else {
                '_'
            }
        })
        .collect();

    (rust_ident, gtest_filter, ignored)
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

    for raw_entry in &test_names {
        // ctest -N の 1 行を (rust 識別子, gtest フィルタ, disabled か) に分解する
        let (rust_name, gtest_filter, ignored) = parse_test_entry(raw_entry);

        // テストバイナリ名を推測
        // StreamRecBufferTest.Foo -> test_stream_rec_buffer
        let binary_name = if let Some(dot_pos) = gtest_filter.find('.') {
            let suite = &gtest_filter[..dot_pos];
            // "Test" サフィックスを除去
            let suite = suite.strip_suffix("Test").unwrap_or(suite);
            format!("test_{}", camel_to_snake(suite))
        } else {
            "test".to_string()
        };

        // DISABLED_ テストは Rust 側でも #[ignore] にし、
        // 強制実行 (cargo test -- --ignored) 時のみ gtest 側でも有効化する
        let ignore_attr = if ignored { "#[ignore]\n" } else { "" };
        let extra_arg = if ignored {
            "\n        .arg(\"--gtest_also_run_disabled_tests\")"
        } else {
            ""
        };

        generated_tests.push_str(&format!(
            r#"{ignore_attr}#[test]
fn {rust_name}() {{
    let test_dir = env!("CPP_TEST_DIR");
    let binary = std::path::Path::new(test_dir).join("{binary_name}");
    let output = std::process::Command::new(&binary)
        .arg("--gtest_filter={gtest_filter}"){extra_arg}
        .output()
        .expect("failed to run test binary");

    if !output.status.success() {{
        eprintln!("{{}}", String::from_utf8_lossy(&output.stdout));
        eprintln!("{{}}", String::from_utf8_lossy(&output.stderr));
        panic!("C++ test {gtest_filter} failed");
    }}
}}

"#,
            ignore_attr = ignore_attr,
            rust_name = rust_name,
            binary_name = binary_name,
            gtest_filter = gtest_filter,
            extra_arg = extra_arg
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
