#![cfg_attr(not(feature = "std"), no_std)]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use core::*;
include!(concat!(env!("OUT_DIR"), "/c2a_core_main.rs"));

pub mod hal;
pub mod system;

#[cfg(feature = "export-src")]
pub fn source_dir() -> std::path::PathBuf {
    std::path::Path::new(env!("CARGO_MANIFEST_DIR")).to_path_buf()
}
