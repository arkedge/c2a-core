use std::io::Write;
use std::path::PathBuf;
use std::process::Command;

/// gtest のテスト名 (例: "StreamRecBufferTest.PushDataSuccess") を、
/// Rust の関数名として有効な小文字識別子に変換する (英数字以外は '_' にする)。
fn sanitize_ident(name: &str) -> String {
    name.chars()
        .map(|c| {
            if c.is_ascii_alphanumeric() {
                c.to_ascii_lowercase()
            } else {
                '_'
            }
        })
        .collect()
}

/// ctest の -R (正規表現) に完全一致で渡せるよう、正規表現メタ文字をエスケープする。
fn regex_escape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        if r".^$*+?()[]{}|\/".contains(c) {
            out.push('\\');
        }
        out.push(c);
    }
    out
}

/// cmake などの外部コマンドを実行し、spawn 失敗・異常終了を分けて報告する。
fn run(cmd: &mut Command, what: &str) {
    let status = cmd
        .status()
        .unwrap_or_else(|e| panic!("failed to spawn {what}: {e}"));
    if !status.success() {
        panic!("{what} failed with status: {status}");
    }
}

fn main() {
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let build_dir = PathBuf::from(&out_dir).join("cpp_tests");
    std::fs::create_dir_all(&build_dir).unwrap();

    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let tests_dir = PathBuf::from(&manifest_dir).join("tests");

    // CMake configure / build
    run(
        Command::new("cmake")
            .args([tests_dir.to_str().unwrap(), "-DCMAKE_BUILD_TYPE=Debug"])
            .current_dir(&build_dir),
        "cmake configure",
    );
    run(
        Command::new("cmake")
            .args(["--build", ".", "-j"])
            .current_dir(&build_dir),
        "cmake build",
    );

    // テスト一覧は JSON で取得する (ctest -N の人間可読出力をパースするより頑健)。
    let output = Command::new("ctest")
        .args(["--show-only=json-v1"])
        .current_dir(&build_dir)
        .output()
        .expect("failed to spawn ctest --show-only=json-v1");
    if !output.status.success() {
        panic!(
            "ctest --show-only=json-v1 failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
    }
    let json: serde_json::Value =
        serde_json::from_slice(&output.stdout).expect("failed to parse ctest json-v1 output");
    let tests = json["tests"]
        .as_array()
        .expect("ctest json-v1: `tests` array not found");

    // 各 C++ テストを個別の Rust テストとして公開する。
    // 実行は ctest に委譲するので、バイナリ名や gtest フィルタの再構築は不要。
    let mut generated = String::new();
    generated
        .push_str("// Auto-generated test wrappers for C++ GoogleTest tests (run via ctest)\n");
    // cargo test はテストを並列実行するため、同一 CMake build ツリーへ複数の ctest が
    // 同時アクセスしてフレークしないよう、ctest 実行を直列化する。
    generated.push_str("static CTEST_MUTEX: std::sync::Mutex<()> = std::sync::Mutex::new(());\n\n");
    for t in tests {
        let name = t["name"]
            .as_str()
            .expect("ctest json-v1: test entry without `name`");

        // DISABLED プロパティを持つテストは Rust 側でも #[ignore] にする。
        let disabled = t["properties"]
            .as_array()
            .map(|props| {
                props.iter().any(|p| {
                    p["name"].as_str() == Some("DISABLED") && p["value"].as_bool() == Some(true)
                })
            })
            .unwrap_or(false);

        let ident = sanitize_ident(name);
        let regex = format!("^{}$", regex_escape(name));
        let ignore_attr = if disabled { "#[ignore]\n" } else { "" };

        generated.push_str(&format!(
            r#"{ignore_attr}#[test]
fn {ident}() {{
    // 同一 build ツリーへの ctest 同時実行を直列化する (poison は無視して継続)。
    let _guard = CTEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
    let build_dir = env!("CPP_TEST_BUILD_DIR");
    let output = std::process::Command::new("ctest")
        .args(["--test-dir", build_dir, "-R", r"{regex}", "--output-on-failure"])
        .output()
        .expect("failed to spawn ctest");
    if !output.status.success() {{
        eprintln!("{{}}", String::from_utf8_lossy(&output.stdout));
        eprintln!("{{}}", String::from_utf8_lossy(&output.stderr));
        panic!("C++ test {name} failed (ctest)");
    }}
}}

"#
        ));
    }

    let generated_path = PathBuf::from(&out_dir).join("generated_tests.rs");
    let mut file = std::fs::File::create(&generated_path).expect("failed to create generated file");
    file.write_all(generated.as_bytes())
        .expect("failed to write generated tests");

    // ctest 実行用にビルドディレクトリを環境変数で渡す。
    println!("cargo:rustc-env=CPP_TEST_BUILD_DIR={}", build_dir.display());
    println!("cargo:rerun-if-changed=tests/");
    // テスト対象本体が変わったら再生成・再ビルドする。
    println!("cargo:rerun-if-changed=driver_super.c");
    println!("cargo:rerun-if-changed=driver_super.h");
    // テストにリンクしている実コア (obc_time) の変更も拾う。
    println!("cargo:rerun-if-changed=../system/time_manager/obc_time.c");
    println!("cargo:rerun-if-changed=../system/time_manager/obc_time.h");
    println!("cargo:rerun-if-changed=../system/time_manager/obc_time_config.h");
}
