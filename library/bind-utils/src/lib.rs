use std::fs;
use std::path::PathBuf;

use bindgen::callbacks::ParseCallbacks;
#[derive(Debug)]
struct Cb;

impl ParseCallbacks for Cb {
    fn process_comment(&self, comment: &str) -> Option<String> {
        let comment = comment.replace("@retval {", "@retval return_value {");
        let dox = doxygen_rs::transform(&comment)
            .trim()
            .replace('[', r"\[")
            .replace(']', r"\]")
            .split('\n')
            .map(|s| s.trim()) // インデントが深いと doctest だと思われる
            .collect::<Vec<_>>()
            .join("\n");
        Some(dox)
    }
}

pub fn bind_c2a_builder() -> bindgen::Builder {
    bindgen::Builder::default()
        .blocklist_type("max_align_t") // https://github.com/servo/rust-bindgen/issues/550
        .ctypes_prefix("core::ffi")
        .use_core()
        .parse_callbacks(Box::new(Cb))
        .default_enum_style(bindgen::EnumVariation::NewType {
            is_bitfield: true,
            is_global: true,
        })
}

pub fn bind_c2a(header_path: PathBuf, gen_rs: PathBuf) {
    let bind = bind_c2a_builder()
        .header(header_path.to_str().unwrap())
        .generate()
        .expect("Unable to generate bindings!");
    bind.write_to_file(&gen_rs)
        .expect("Couldn't write bindings!");

    c2a_link_hack(gen_rs);
}

pub fn c2a_link_hack(gen_rs: PathBuf) {
    // link hack
    let bind = fs::read_to_string(&gen_rs).unwrap();
    let bind = bind.replace(
        "extern \"C\" {",
        "#[cfg_attr(not(feature=\"no-c2a-link\"),link(name = \"C2A\", kind = \"static\", modifiers = \"-bundle\"))]\nextern \"C\" {",
    );
    fs::write(gen_rs, bind).unwrap();
}
