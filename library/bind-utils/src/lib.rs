use std::fs;
use std::path::PathBuf;

use bindgen::callbacks::ParseCallbacks;
#[derive(Debug)]
struct Cb;

impl ParseCallbacks for Cb {
    fn process_comment(&self, comment: &str) -> Option<String> {
        let comment = comment.replace("@retval {", "@retval return_value {");
        // doxygen-rs は @param[in] name: の "name:" をパースできないためコロンを除去
        let comment = strip_doxygen_param_colon(&comment);
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

/// `@param[in] name: desc` や `@param[in]: desc` のコロンを除去する。
///
/// doxygen-rs はパラメータ名やディレクション指定の直後にコロンがある形式を
/// パースできないため、bindgen に渡す前にコロンを取り除く。
fn strip_doxygen_param_colon(comment: &str) -> String {
    let mut result = String::with_capacity(comment.len());
    for line in comment.lines() {
        if !result.is_empty() {
            result.push('\n');
        }
        // @param を含む行のみ処理
        if let Some(param_pos) = line.find("@param") {
            let after_param = &line[param_pos + "@param".len()..];
            // @param[in]/[out]/[in,out] をスキップ
            let rest = if after_param.starts_with('[') {
                match after_param.find(']') {
                    Some(bracket_end) => &after_param[bracket_end + 1..],
                    None => {
                        result.push_str(line);
                        continue;
                    }
                }
            } else {
                after_param
            };
            // ケース1: @param[in]: desc (パラメータ名なし、コロンが直後)
            let rest_trimmed = rest.trim_start();
            if rest_trimmed.starts_with(':') {
                let colon_pos = param_pos
                    + "@param".len()
                    + (after_param.len() - rest.len())
                    + (rest.len() - rest_trimmed.len());
                result.push_str(&line[..colon_pos]);
                result.push_str(&line[colon_pos + 1..]);
                continue;
            }
            // ケース2: @param[in] name: desc (パラメータ名の末尾にコロン)
            let name_end = rest_trimmed
                .find(|c: char| !c.is_alphanumeric() && c != '_')
                .unwrap_or(rest_trimmed.len());
            if name_end > 0 && rest_trimmed.as_bytes().get(name_end) == Some(&b':') {
                let colon_pos = param_pos
                    + "@param".len()
                    + (after_param.len() - rest.len())
                    + (rest.len() - rest_trimmed.len())
                    + name_end;
                result.push_str(&line[..colon_pos]);
                result.push_str(&line[colon_pos + 1..]);
            } else {
                result.push_str(line);
            }
        } else {
            result.push_str(line);
        }
    }
    result
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
