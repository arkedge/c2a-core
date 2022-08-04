#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re


class C2aEnum:
    def __init__(self, c2a_src_path, encoding):
        self.path = c2a_src_path
        self.encoding = encoding
        self.search_dirs = [
            "/src_user/",
            "/src_core/Applications/",
            "/src_core/Drivers/",
            "/src_core/IfWrapper/",
            "/src_core/Library/",
            "/src_core/System/",
            "/src_core/TlmCmd/",
        ]

        self._get_all_enum()

    def _get_all_enum(self):
        for search_dir in self.search_dirs:
            search_dir = self.path + search_dir

            for root, dirs, files in os.walk(search_dir):
                for file in files:
                    ext = (os.path.splitext(file))[1]
                    if ext != ".h" and ext != ".c" and ext != ".hpp" and ext != ".cpp":
                        continue
                    path = root + r"/" + file
                    path = path.replace("\\", "/")
                    enum_codes = self._search_enum_from_file(path)

                    for code_lines in enum_codes:
                        self._load_enum(code_lines)

    def _search_enum_from_file(self, path):
        ret = []
        with open(path, encoding=self.encoding) as f:
            code_lines = f.read().split("\n")

        code_lines = self._delete_preprocessor(code_lines)
        code_lines = self._delete_multiline_comment(code_lines)

        p_enum_begin = re.compile(r"^ *typedef +enum")
        p_enum_end = re.compile(r"^ *} +\w+")

        is_in_enum = False
        enum = []
        for line in code_lines:
            if is_in_enum:
                if p_enum_end.search(line):
                    ret.append(enum)
                    enum = []
                    is_in_enum = False
                else:
                    enum.append(line)
            else:
                if p_enum_begin.search(line):
                    is_in_enum = True

        return ret

    def _delete_preprocessor(self, code_lines):
        p = re.compile(r"^#")
        return [line for line in code_lines if not p.search(line)]

    def _delete_multiline_comment(self, code_lines):
        # TODO: #if 0 - #endif のものはのこってしまう．
        code = "\n".join(code_lines)
        ret = ""
        pre = " "
        is_in_comment = False
        for char in code:
            if is_in_comment:
                if pre == "*" and char == "/":
                    is_in_comment = False
            else:
                if pre == "/" and char == "*":
                    is_in_comment = True
                    # 前の1文字を削除
                    ret = ret[:-1]
                else:
                    ret += char

            pre = char

        return ret.split("\n")

    def _load_enum(self, code_lines):
        # TODO: ここの最初の 2 空白については要議論
        p_with_id = re.compile(r"^  (\w+) += +(\w+)")
        p_without_id = re.compile(r"^  (\w+)")

        last_enum_id = -1
        for line in code_lines:
            m_with_id = p_with_id.search(line)
            m_without_id = p_without_id.search(line)
            if not m_with_id and not m_without_id:
                continue

            if m_with_id:
                enum_name = m_with_id.group(1)
                enum_id = m_with_id.group(2)
            else:
                enum_name = m_without_id.group(1)
                enum_id = str(last_enum_id + 1)

            if enum_id[:2] == "0x":
                enum_id = int(enum_id, base=16)
            else:
                enum_id = int(enum_id, base=10)

            self.__setattr__(enum_name, enum_id)
            last_enum_id = enum_id


def load_enum(c2a_src_path, encoding) -> C2aEnum:
    c2a_enum = C2aEnum(c2a_src_path, encoding)
    return c2a_enum


if __name__ == "__main__":
    # 単なる動作確認用
    # c2a_enum = load_enum(os.path.dirname(__file__) + "/../../..", "utf-8")
    c2a_enum = load_enum(
        os.path.dirname(__file__) + "/../../c2a-core/Examples/minimum_user/src", "utf-8"
    )
    for key, value in c2a_enum.__dict__.items():
        print(key, " : \t", value)
