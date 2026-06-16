fn main() {
    // Auto-detect target architecture and set appropriate build flags
    let target_arch = std::env::var("CARGO_CFG_TARGET_ARCH").unwrap_or_else(|_| "x86_64".to_string());
    let build_for_32bit = match target_arch.as_str() {
        "i686" | "i586" | "arm" => "ON",
        _ => "OFF",
    };
    
    // Build C2A & link
    let mut c2a_cmake = cmake::Config::new(".");
    
    // Use system default compiler for macOS
    if cfg!(target_os = "macos") {
        c2a_cmake.define("CMAKE_C_COMPILER", "clang");
        c2a_cmake.define("CMAKE_CXX_COMPILER", "clang++");
    }
    
    let libc2a = c2a_cmake
        .very_verbose(true)
        .define("C2A_BUILD_FOR_32BIT", build_for_32bit)
        .define("C2A_BUILD_AS_C99", "ON")
        .define("C2A_BUILD_FOR_SILS", "ON")
        .define("C2A_USE_SCI_COM_WINGS", "OFF")
        .build_target("C2A");

    println!("cargo:rerun-if-changed=./src/src_core");
    println!("cargo:rerun-if-changed=./src/src_user");

    let libc2a = libc2a.build();
    println!(
        "cargo:rustc-link-search=native={}/build", // no install step in libC2A
        libc2a.display()
    );
    println!("cargo:rustc-link-lib=static=C2A");
}
