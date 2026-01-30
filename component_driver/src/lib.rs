//! C2A Component Driver unit tests
//!
//! This crate wraps C++ GoogleTest tests for the component_driver module.
//! Run with `cargo test` to execute all C++ unit tests.
//!
//! Each C++ test case is exposed as an individual Rust test.

#[cfg(test)]
mod tests {
    include!(concat!(env!("OUT_DIR"), "/generated_tests.rs"));
}
