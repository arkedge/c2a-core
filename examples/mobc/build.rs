fn main() {
    // Cargo のビルド出力ディレクトリを取得
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let out_dir_path = std::path::PathBuf::from(&out_dir);

    // C2A core のソースディレクトリを取得
    let c2a_core_dir = if cfg!(feature = "use-local-c2a-core") {
        // ローカル開発用: 従来のシンボリックリンク方式
        std::path::PathBuf::from("./src/src_core")
            .canonicalize()
            .expect("Failed to canonicalize C2A core directory (symlink)")
    } else {
        // Cargo 経由: c2a-core crate のソースディレクトリを取得
        let core_dir = c2a_core::source_dir();

        // ビルドディレクトリ内に src_core シンボリックリンクを作成
        // これにより CMake が <src_core/...> を解決できるようになる
        let src_core_link = out_dir_path.join("src_core");

        // 既存のシンボリックリンクを削除（古いパスを指している可能性があるため）
        if src_core_link.exists() || src_core_link.read_link().is_ok() {
            let _ = std::fs::remove_file(&src_core_link);
        }

        // 新しいシンボリックリンクを作成
        #[cfg(unix)]
        std::os::unix::fs::symlink(&core_dir, &src_core_link)
            .expect("Failed to create src_core symlink in build directory");

        #[cfg(windows)]
        std::os::windows::fs::symlink_dir(&core_dir, &src_core_link)
            .expect("Failed to create src_core symlink in build directory");

        core_dir
    };

    // C2A user のソースディレクトリ
    let c2a_user_dir = std::path::PathBuf::from("./src/src_user");
    let c2a_user_dir = c2a_user_dir
        .canonicalize()
        .expect("Failed to canonicalize C2A user directory");

    // ビルドディレクトリ内に src_user シンボリックリンクを作成
    // これにより CMake が <src_user/...> を解決できるようになる
    // これで実際のディレクトリ名を src_user に限定する必要がなくなる
    let src_user_link = out_dir_path.join("src_user");

    // 既存のシンボリックリンクを削除（古いパスを指している可能性があるため）
    if src_user_link.exists() || src_user_link.read_link().is_ok() {
        let _ = std::fs::remove_file(&src_user_link);
    }

    // 新しいシンボリックリンクを作成
    #[cfg(unix)]
    std::os::unix::fs::symlink(&c2a_user_dir, &src_user_link)
        .expect("Failed to create src_user symlink in build directory");

    #[cfg(windows)]
    std::os::windows::fs::symlink_dir(&c2a_user_dir, &src_user_link)
        .expect("Failed to create src_user symlink in build directory");

    println!("cargo:rerun-if-changed=./src/src_core");
    println!("cargo:rerun-if-changed=./src/src_user");

    // C2A core の crate version を取得（crates.io からの場合に git revision の代わりに使用）
    let c2a_core_version = std::env::var("DEP_C2A_CORE_VERSION")
        .or_else(|_| std::env::var("CARGO_PKG_VERSION"))
        .unwrap_or_else(|_| "0.0.0".to_string());

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
        // ビルドディレクトリを include path に追加するため CMake に渡す
        .define("C2A_BUILD_OUT_DIR", &out_dir)
        // C2A core の version を渡す（git revision が取得できない場合の代替）
        .define("C2A_CORE_VERSION", &c2a_core_version)
        // VSCode の IntelliSense 用に compile_commands.json を生成
        .define("CMAKE_EXPORT_COMPILE_COMMANDS", "ON")
        .build_target("C2A");

    let libc2a = libc2a.build();
    println!(
        "cargo:rustc-link-search=native={}/build", // no install step in libC2A
        libc2a.display()
    );
    println!("cargo:rustc-link-lib=static=C2A");

    // VSCode の IntelliSense 用に compile_commands.json へのシンボリックリンクを作成
    let compile_commands_src = libc2a.join("build/compile_commands.json");
    let compile_commands_dst = std::path::PathBuf::from("./compile_commands.json");

    if compile_commands_src.exists() {
        // 既存のシンボリックリンクを削除
        if compile_commands_dst.exists() || compile_commands_dst.read_link().is_ok() {
            let _ = std::fs::remove_file(&compile_commands_dst);
        }

        // 新しいシンボリックリンクを作成
        #[cfg(unix)]
        {
            let _ = std::os::unix::fs::symlink(&compile_commands_src, &compile_commands_dst);
        }

        #[cfg(windows)]
        {
            let _ = std::os::windows::fs::symlink_file(&compile_commands_src, &compile_commands_dst);
        }
    }

    // VSCode の IntelliSense 用に .vscode/c_cpp_properties.json を動的に生成/更新
    let vscode_dir = std::path::PathBuf::from("./.vscode");
    std::fs::create_dir_all(&vscode_dir).ok();

    let c_cpp_properties_path = vscode_dir.join("c_cpp_properties.json");
    let c2a_core_path = format!("{}/**", c2a_core_dir.display());
    let out_dir_path = format!("{}/**", out_dir);

    // 既存の c_cpp_properties.json を読み込むか、デフォルトを作成
    let mut config: serde_json::Value = if c_cpp_properties_path.exists() {
        let content = std::fs::read_to_string(&c_cpp_properties_path).ok();
        content
            .and_then(|s| serde_json::from_str(&s).ok())
            .unwrap_or_else(|| create_default_c_cpp_properties())
    } else {
        create_default_c_cpp_properties()
    };

    // configurations[0].includePath に c2a_core と out_dir を追加（存在しない場合のみ）
    if let Some(configurations) = config.get_mut("configurations").and_then(|c| c.as_array_mut()) {
        if let Some(first_config) = configurations.get_mut(0) {
            if let Some(include_path) = first_config.get_mut("includePath").and_then(|p| p.as_array_mut()) {
                // 古い out_dir パス（target/.../out/** パターン）を削除
                include_path.retain(|p| {
                    if let Some(path_str) = p.as_str() {
                        !path_str.contains("/target/") || !path_str.contains("/out/**")
                    } else {
                        true
                    }
                });

                // c2a_core_path が存在しない場合は追加
                if !include_path.iter().any(|p| p.as_str() == Some(&c2a_core_path)) {
                    include_path.push(serde_json::Value::String(c2a_core_path.clone()));
                }
                // 新しい out_dir_path を追加
                include_path.push(serde_json::Value::String(out_dir_path.clone()));
            }

            // compileCommands が設定されていない場合は追加
            if first_config.get("compileCommands").is_none() {
                first_config["compileCommands"] = serde_json::Value::String("${workspaceFolder}/compile_commands.json".to_string());
            }
        }
    }

    // 整形して書き込み
    if let Ok(pretty_json) = serde_json::to_string_pretty(&config) {
        std::fs::write(&c_cpp_properties_path, pretty_json).ok();
    }
}

fn create_default_c_cpp_properties() -> serde_json::Value {
    serde_json::json!({
        "configurations": [
            {
                "name": "Linux",
                "includePath": [
                    "${workspaceFolder}/**",
                    "${workspaceFolder}/src/src_user/**"
                ],
                "defines": [
                    "SHOW_DEBUG_PRINT_ON_SILS"
                ],
                "compilerPath": "/usr/bin/gcc",
                "cStandard": "c99",
                "cppStandard": "c++17",
                "intelliSenseMode": "linux-gcc-x64",
                "compileCommands": "${workspaceFolder}/compile_commands.json"
            }
        ],
        "version": 4
    })
}
