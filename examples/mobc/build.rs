fn main() {
    // C2A core のソースディレクトリを取得
    let c2a_core_dir = if cfg!(feature = "use-local-c2a-core") {
        // ローカル開発用: 従来のシンボリックリンク方式
        std::path::PathBuf::from("./src/src_core")
            .canonicalize()
            .expect("Failed to canonicalize C2A core directory (symlink)")
    } else {
        // Cargo 経由: c2a-core crate のソースディレクトリを取得
        c2a_core::source_dir()
    };

    // C2A user のソースディレクトリ
    let c2a_user_dir = std::path::PathBuf::from("./src/src_user");
    let c2a_user_dir = c2a_user_dir
        .canonicalize()
        .expect("Failed to canonicalize C2A user directory");

    println!("cargo:rerun-if-changed=./src/src_core");
    println!("cargo:rerun-if-changed=./src/src_user");

    // Build C2A & link
    let mut c2a_cmake = cmake::Config::new(".");
    let libc2a = c2a_cmake
        .very_verbose(true)
        .define("C2A_BUILD_FOR_32BIT", "OFF")
        .define("C2A_BUILD_AS_C99", "ON")
        .define("C2A_BUILD_FOR_SILS", "ON")
        .define("C2A_USE_SCI_COM_WINGS", "OFF")
        // C2A core と user のディレクトリを明示的に指定
        .define("C2A_CORE_DIR", c2a_core_dir.to_str().unwrap())
        .define("C2A_USER_DIR", c2a_user_dir.to_str().unwrap())
        .build_target("C2A");

    let libc2a = libc2a.build();
    println!(
        "cargo:rustc-link-search=native={}/build", // no install step in libC2A
        libc2a.display()
    );
    println!("cargo:rustc-link-lib=static=C2A");
}
