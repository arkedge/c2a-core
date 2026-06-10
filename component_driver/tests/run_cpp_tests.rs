//! C++ (GoogleTest) テストを CMake + ctest で実行するラッパ。
//!
//! build script ではなくテスト実行時に CMake を走らせることで、`cargo build` /
//! `cargo clippy` など通常の workspace 操作を CMake やネットワークに依存させない。
//! テストの列挙・並列実行・差分ビルドはすべて CMake / ctest に委譲する。
//! 個別のテストケースの結果は ctest の出力 (失敗時に表示) を参照。

use std::path::PathBuf;
use std::process::Command;

/// 外部コマンドを実行し、失敗時は stdout/stderr を表示して panic する。
fn run(cmd: &mut Command, what: &str) {
    let output = cmd
        .output()
        .unwrap_or_else(|e| panic!("failed to spawn {what}: {e}"));
    if !output.status.success() {
        eprintln!("{}", String::from_utf8_lossy(&output.stdout));
        eprintln!("{}", String::from_utf8_lossy(&output.stderr));
        panic!("{what} failed with status: {}", output.status);
    }
}

#[test]
fn run_cpp_tests() {
    let manifest_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source_dir = manifest_dir.join("tests");

    // workspace の target/ 以下に build ツリーを置き、CMake の差分ビルドを効かせる
    let target_dir = std::env::var_os("CARGO_TARGET_DIR")
        .map(PathBuf::from)
        .unwrap_or_else(|| manifest_dir.parent().unwrap().join("target"));
    let build_dir = target_dir.join("component_driver_cpp_tests");
    std::fs::create_dir_all(&build_dir).expect("failed to create build dir");

    // flight 環境 (workspace の i686 target) に合わせ 32bit でビルドする。
    // multilib が無い環境では C2A_CPP_TEST_CFLAGS で上書きできる (例: 空文字)。
    let flags = std::env::var("C2A_CPP_TEST_CFLAGS").unwrap_or_else(|_| "-m32".to_string());

    run(
        Command::new("cmake")
            .arg(&source_dir)
            .arg("-DCMAKE_BUILD_TYPE=Debug")
            .arg(format!("-DCMAKE_C_FLAGS={flags}"))
            .arg(format!("-DCMAKE_CXX_FLAGS={flags}"))
            .current_dir(&build_dir),
        "cmake configure",
    );
    run(
        Command::new("cmake")
            .args(["--build", ".", "-j"])
            .current_dir(&build_dir),
        "cmake build",
    );

    let jobs = std::thread::available_parallelism().map_or(1, |n| n.get());
    run(
        Command::new("ctest")
            .args(["--output-on-failure", "-j", &jobs.to_string()])
            .current_dir(&build_dir),
        "ctest",
    );
}
