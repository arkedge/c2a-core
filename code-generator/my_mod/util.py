# coding: UTF-8
"""
util
"""

import subprocess
import sys
import os
import hashlib


def GenerateSettingNote(settings):
    note = ""
    note += " * @note  このコードは自動生成されています！\n"
    note += " * @note  コード生成元 tlm-cmd-db:\n"
    note += " *          repository:    " + GetRepo_(settings["path_to_db"]) + "\n"
    note += " *          CSV files MD5: " + GetDbHash_(settings["path_to_db"]) + "\n"
    note += " * @note  コード生成パラメータ:\n"
    note += " *          db_prefix:             " + settings["db_prefix"] + "\n"
    note += " *          tlm_id_range:          "
    note += "[" + settings["tlm_id_range"][0] + ", " + settings["tlm_id_range"][1] + "]\n"
    note += " *          is_cmd_prefixed_in_db: " + str(settings["is_cmd_prefixed_in_db"]) + "\n"
    note += " *          input_file_encoding:   " + settings["input_file_encoding"] + "\n"
    note += " *          output_file_encoding:  " + settings["output_file_encoding"] + "\n"
    # is_main_obc については，生成状況によって異なるので出力しない
    # path_to_src, path_to_db については，実行環境によって異なるので出力しない

    return note


def GenerateSubObcSettingNote(settings, obc_idx):
    sub_obc_settings = settings["other_obc_data"][obc_idx]

    note = ""
    note += " * @note  このコードは自動生成されています！\n"
    note += " * @note  コード生成元 tlm-cmd-db:\n"
    note += " *          repository:     "
    note += GetRepo_(sub_obc_settings["path_to_db"]) + "\n"
    note += " *          CSV files MD5:  " + GetDbHash_(settings["path_to_db"]) + "\n"
    note += " *          db commit hash: " + GetCommitHash_(sub_obc_settings["path_to_db"]) + "\n"
    note += " * @note  コード生成パラメータ:\n"
    note += " *          name:                    " + sub_obc_settings["name"] + "\n"
    note += " *          db_prefix:               " + sub_obc_settings["db_prefix"] + "\n"
    note += (
        " *          tlm_id_range:            "
        + "["
        + sub_obc_settings["tlm_id_range"][0]
        + ", "
        + sub_obc_settings["tlm_id_range"][1]
        + "]\n"
    )
    note += (
        " *          is_cmd_prefixed_in_db:   "
        + str(sub_obc_settings["is_cmd_prefixed_in_db"])
        + "\n"
    )
    note += " *          input_file_encoding:     " + sub_obc_settings["input_file_encoding"] + "\n"
    note += " *          max_tlm_num:             " + str(sub_obc_settings["max_tlm_num"]) + "\n"
    note += " *          driver_path:             " + sub_obc_settings["driver_path"] + "\n"
    note += " *          driver_type:             " + sub_obc_settings["driver_type"] + "\n"
    note += " *          driver_name:             " + sub_obc_settings["driver_name"] + "\n"
    note += (
        " *          code_when_tlm_not_found: " + sub_obc_settings["code_when_tlm_not_found"] + "\n"
    )
    # path_to_db については，実行環境によって異なるので出力しない

    return note


def GetCommitHash_(path):
    try:
        result = subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=path, text=True, capture_output=True, check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        print("Warn: failed to get commit hash(" + path + ")", file=sys.stderr)
        return "unknown"


# Python 3.8 には str.removeprefix() が無い
def RemovePrefix_(text, prefix):
    if text.startswith(prefix):
        text = text[len(prefix) :]
    return text


def GetRepo_(path):
    # GitHub などの場合: github.com/user/repo のようにする
    # 取得に失敗した場合（Git 管理していないものなど）: unknown を返し，warning を出す

    try:
        subprocess.run(["git", "--version"], capture_output=True, check=True)
    except subprocess.CalledProcessError:
        print("Warn: failed to execute git command", file=sys.stderr)
        return "unknown/unknown/unknown"

    try:
        result = subprocess.run(
            ["git", "remote"], cwd=path, text=True, capture_output=True, check=True
        )
        remote = result.stdout.split("\n")[0]  # 最初の remote を取得

        if not remote:
            print("Warn: failed to get git remote", file=sys.stderr)
            return "unknown/unknown/unknown"

        remote_url = subprocess.run(
            ["git", "remote", "get-url", remote],
            cwd=path,
            text=True,
            capture_output=True,
            check=True,
        ).stdout.strip()

        # HTTPS と SSH の remote URL の差異を吸収（削除）
        remote_url = RemovePrefix_(remote_url, "git@")
        remote_url = RemovePrefix_(remote_url, "https://")
        remote_url = remote_url.replace(":", "/")

        # URLの末尾に.gitがなければ追加
        if not remote_url.endswith(".git"):
            remote_url += ".git"

        return remote_url
    except subprocess.CalledProcessError:
        print("Warn: failed to execute: git remote", file=sys.stderr)
        return "unknown/unknown/unknown"


# 入力 DB の csv をファイル名でソートし MD5 を計算，その MD5 をすべて cat して MD5 を計算したものを返す
def GetDbHash_(path):
    csv_files_info = FindCsvFilesAndCalculateMd5_(path)

    # ファイル名でソートし，MD5 を結合したのち，その MD5 を計算
    sorted_info = sorted(csv_files_info, key=lambda x: x["filepath"])
    concatenated_md5s = "".join(info["md5"] for info in sorted_info)
    final_md5 = hashlib.md5(concatenated_md5s.encode()).hexdigest()
    return final_md5


def CalcMd5_(path):
    # Windows 環境で改行コードが CRLF になっているとハッシュ値が変わってしまう
    # そのため，MD5 の計算は CRLF -> LF してから行う
    with open(path, "r", encoding="utf-8") as file:
        content = file.read()
    content_lf = content.replace("\r\n", "\n")
    return hashlib.md5(content_lf.encode("utf-8")).hexdigest()


def FindCsvFilesAndCalculateMd5_(path):
    csv_files_info = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if name.endswith(".csv"):
                file_path = os.path.join(root, name)
                md5 = CalcMd5_(file_path)
                csv_files_info.append({"filepath": file_path, "md5": md5})
    return csv_files_info
