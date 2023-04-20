# coding: UTF-8
"""
python 3.7以上を要求
"""

import json
import sys

import my_mod.load_db
import my_mod.cmd_def
import my_mod.tlm_def
import my_mod.tlm_buffer


# import pprint
# import os.path
# import msvcrt               # Enter不要な入力用
# import subprocess


# 環境変数
DEBUG = 0
# 0 : Release
# 1 : all
SETTING_FILE_PATH = "settings.json"


def main():
    with open(SETTING_FILE_PATH, mode="r") as fh:
        settings = json.load(fh)
    # print(settings["c2a_root_dir"]);

    cmd_db = my_mod.load_db.LoadCmdDb(settings)
    tlm_db = my_mod.load_db.LoadTlmDb(settings)
    # pprint.pprint(cmd_db)
    # pprint.pprint(tlm_db)
    # print(tlm_db)

    my_mod.cmd_def.GenerateCmdDef(settings, cmd_db["sgc"])
    my_mod.cmd_def.GenerateBctDef(settings, cmd_db["bct"])
    my_mod.tlm_def.GenerateTlmDef(settings, tlm_db["tlm"])

    if settings["is_main_obc"]:
        my_mod.cmd_def.GenerateOtherObcCmdDef(settings, cmd_db["other_obc"])
        my_mod.tlm_def.GenerateOtherObcTlmDef(settings, tlm_db["other_obc"])
        my_mod.tlm_buffer.GenerateTlmBuffer(settings, tlm_db["other_obc"])

    print("Completed!")
    sys.exit(0)


if __name__ == "__main__":
    main()
