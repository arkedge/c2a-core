# coding: UTF-8
"""
python 3.7以上を要求
"""

import json
import csv
import os
import re           # 正規表現
import pprint

# import sys
# import os
# import os.path
# import msvcrt               # Enter不要な入力用
# import subprocess


# 環境変数
DEBUG = 0
# 0 : Release
# 1 : all
SETTING_FILE_PATH = "settings.json"


def main():
    with open(SETTING_FILE_PATH, mode='r') as fh:
        settings = json.load(fh)
    # print(settings["c2a_root_dir"]);

    cmd_db = LoadCmdCSV(settings)
    tlm_db = LoadTlmCSV(settings)
    # pprint.pprint(cmd_db)
    # pprint.pprint(tlm_db)
    # print(tlm_db)

    GenerateCmdDef(settings, cmd_db['sgc'])
    GenerateBctDef(settings, cmd_db['bct'])




def LoadCmdCSV(settings):
    cmd_db_path = settings["c2a_root_dir"] + r"src_user/Settings/CmdTlm/DataBase/CMD_DB/"
    sgc_db_path = cmd_db_path + settings["db_prefix"] + "_CMD_DB_CMD_DB.csv"        # single cmd
    bct_db_path = cmd_db_path + settings["db_prefix"] + "_CMD_DB_BCT.csv"       # block cmd table

    with open(sgc_db_path, mode='r', encoding='shift_jis') as fh:
        reader = csv.reader(fh)
        sgc_db = [row for row in reader]
    with open(bct_db_path, mode='r', encoding='shift_jis') as fh:
        reader = csv.reader(fh)
        bct_db = [row for row in reader]

    # print(sgc_db)
    # print(bct_db)
    return {'sgc': sgc_db, 'bct': bct_db}


def LoadTlmCSV(settings):
    tlm_db_path = settings["c2a_root_dir"] + r"src_user/Settings/CmdTlm/DataBase/TLM_DB/calced_data/"

    tlm_names = [file for file in os.listdir(tlm_db_path) if file.endswith(".csv")]
    regex = r"^" + settings["db_prefix"] + "_TLM_DB_"
    tlm_names = [re.sub(regex, "", file) for file in tlm_names]
    tlm_names = [re.sub(".csv$", "", file) for file in tlm_names]
    # pprint.pprint(tlm_names)
    # print(len(tlm_names))

    tlm_db = []

    for tlm_name in tlm_names:
        tlm_sheet_path = tlm_db_path + settings["db_prefix"] + "_TLM_DB_" + tlm_name + ".csv"
        with open(tlm_sheet_path, mode='r', encoding='shift_jis') as fh:
            reader = csv.reader(fh)
            sheet = [row for row in reader]
            # pprint.pprint(sheet)
            # print(sheet)
            tlm_id = sheet[1][2]      # テレメIDを取得．マジックナンバーで指定してしまってる．
            tlm_db.append({'tlm_id': tlm_id, 'tlm_name': tlm_name, 'data': sheet})
            # tlm_db.append({'tlm_id': tlm_id, 'tlm_name': tlm_name, 'data': 1})

    tlm_db.sort(key=lambda x: x['tlm_id'])
    return tlm_db;


def GenerateCmdDef(settings, sgc_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/CmdTlm/"
    output_file_name_base = "CommandDefinitions"

    DATA_SART_ROW = 3

    body_c = ""
    body_h = ""
    # "  cmd_table_[Cmd_CODE_NOP].cmd_func = Cmd_NOP;"
    # "  Cmd_CODE_NOP = 0x0000,"
    for i in range(DATA_SART_ROW, len(sgc_db)):
        if sgc_db[i][0] != "":                                 # Comment
            continue
        if sgc_db[i][0] == "" and sgc_db[i][1] == "":         # CommentもNameも空白なら打ち切り
            break
        # print(sgc_db[i][1])
        cmd_name = sgc_db[i][1]
        cmd_code = cmd_name.replace("Cmd_", "Cmd_CODE_")
        # print(cmd_name)
        # print(cmd_code)
        body_c += "  cmd_table_[" + cmd_code + "].cmd_func = " + cmd_name + ";\n"
        body_h += "  " + cmd_code + " = " + sgc_db[i][3] + ",\n"

    output_c = ""
    output_h = ""

    output_c += '''
#pragma section REPRO
/**
 * @file   CommandDefinitions.c
 * @brief  コマンド定義
 * @author 鈴本 遼
 * @date   2020/08/23
 */
#include "CommandDefinitions.h"
#include "CommandSource.h"

/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##

void CA_load_cmd_table(CmdInfo cmd_table_[CMD_MAX_CMDS])
{
'''[1:]         # 最初の改行を除く

    output_c += body_c

    output_c += '''
}

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/

#pragma section
'''[1:]         # 最初の改行を除く


    output_h += '''
/**
 * @file   CommandDefinitions.h
 * @brief  コマンド定義
 * @author 鈴本 遼
 * @date   2020/08/23
 */
#ifndef COMMAND_DEFINITIONS_H_
#define COMMAND_DEFINITIONS_H_

#include "../../src_core/CmdTlm/CommandAnalyze.h"

typedef enum
{
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##

'''[1:]         # 最初の改行を除く

    output_h += body_h

    output_h += '''

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
  Cmd_ID_MAX
} CmdCode;

void CA_load_cmd_table(CmdInfo cmd_table_[CMD_MAX_CMDS]);

#endif
'''[1:]         # 最初の改行を除く

    with open(output_file_path + output_file_name_base + ".c", mode='w', encoding='shift_jis') as fh:
        fh.write(output_c)
    with open(output_file_path + output_file_name_base + ".h", mode='w', encoding='shift_jis') as fh:
        fh.write(output_h)


def GenerateBctDef(settings, bct_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/CmdTlm/"
    output_file_name = "BlockCommandDefinitions.h"


if __name__ == '__main__':
    main()



