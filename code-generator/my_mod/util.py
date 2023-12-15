# coding: UTF-8
"""
util
"""

import subprocess
import re


def GenerateSettingNote(settings):
    note = ""
    note += " * @note  このコードは自動生成されています！\n"
    note += " * @note  コード生成 tlm-cmd-db:\n"
    note += " *          repository:     "
    note += GetRepoName_(settings["path_to_db"])
    note += "\n"
    note += " *          db commit hash: "
    note += "xxxx"
    # note += GetCommitHash_(settings["path_to_db"])
    note += "\n"
    note += " * @note  コード生成パラメータ:\n"
    note += " *          db_prefix:             "
    note += settings["db_prefix"]
    note += "\n"
    note += " *          tlm_id_range:          "
    note += "[" + settings["tlm_id_range"][0] + ", " + settings["tlm_id_range"][1] + "]"
    note += "\n"
    note += " *          is_cmd_prefixed_in_db: "
    note += str(settings["is_cmd_prefixed_in_db"])
    note += "\n"
    note += " *          input_file_encoding:   "
    note += settings["input_file_encoding"]
    note += "\n"
    note += " *          output_file_encoding:  "
    note += settings["output_file_encoding"]
    note += "\n"
    # is_main_obc については，生成状況によって異なるので出力しない
    # path_to_src, path_to_db については，実行環境によって異なるので出力しない

    return note


def GenerateSubObcSettingNote(settings, obc_idx):
    sub_obc_settings = settings["other_obc_data"][obc_idx]

    note = ""
    note += " * @note  このコードは自動生成されています！\n"
    note += " * @note  コード生成 tlm-cmd-db:\n"
    note += " *          repository:     "
    note += GetRepoName_(sub_obc_settings["path_to_db"])
    note += "\n"
    note += " *          db commit hash: "
    note += GetCommitHash_(sub_obc_settings["path_to_db"])
    note += "\n"
    note += " * @note  コード生成パラメータ:\n"
    note += " *          name:                    "
    note += sub_obc_settings["name"]
    note += "\n"
    note += " *          db_prefix:               "
    note += sub_obc_settings["db_prefix"]
    note += "\n"
    note += " *          tlm_id_range:            "
    note += "[" + sub_obc_settings["tlm_id_range"][0] + ", " + sub_obc_settings["tlm_id_range"][1] + "]"
    note += "\n"
    note += " *          is_cmd_prefixed_in_db:   "
    note += str(sub_obc_settings["is_cmd_prefixed_in_db"])
    note += "\n"
    note += " *          input_file_encoding:     "
    note += sub_obc_settings["input_file_encoding"]
    note += "\n"
    note += " *          max_tlm_num:             "
    note += str(sub_obc_settings["max_tlm_num"])
    note += "\n"
    note += " *          driver_path:             "
    note += sub_obc_settings["driver_path"]
    note += "\n"
    note += " *          driver_type:             "
    note += sub_obc_settings["driver_type"]
    note += "\n"
    note += " *          driver_name:             "
    note += sub_obc_settings["driver_name"]
    note += "\n"
    note += " *          code_when_tlm_not_found: "
    note += sub_obc_settings["code_when_tlm_not_found"]
    note += "\n"
    # path_to_db については，実行環境によって異なるので出力しない

    return note


def GetCommitHash_(path):
    try:
        result = subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=path, text=True, capture_output=True, check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        return "0000000000000000000000000000000000000000"


def GetRepoName_(path):
    try:
        # GitリモートURLを取得
        result = subprocess.run(["git", "remote", "-v"], cwd=path, text=True, capture_output=True, check=True)
        url = result.stdout.split('\n')[0].split('\t')[1].split(' ')[0]  # 最初のリモートURLを取得

        # URLの末尾に.gitがなければ追加
        if not url.endswith(".git"):
            url += ".git"

        # URLからユーザー名とリポジトリ名を抽出（HTTPSとSSHの両方に対応）
        match = re.search(r'(?:github\.com[:/])(.+)/(.+)\.git', url)
        if match:
            return f"{match.group(1)}/{match.group(2)}"
        else:
            return "User/Repository name not found"
    except subprocess.CalledProcessError:
        return "User/Repository name not found"
