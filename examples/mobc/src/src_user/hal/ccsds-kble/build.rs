use std::env;
use std::path::PathBuf;

use c2a_bind_utils::*;

fn main() {
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    let bind = bind_c2a_builder()
        .header("../ccsds_user.h")
        .clang_arg("-I../../../")
        .generate()
        .expect("Unable to generate bindings!");
    bind.write_to_file(out_dir.join("ccsds_user.rs"))
        .expect("Couldn't write bindings!");

    println!("cargo:rerun-if-changed=../ccsds_user.h");
}
