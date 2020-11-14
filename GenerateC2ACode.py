# coding: UTF-8
"""
python 3.7以上を要求
"""

import json
import csv
import os
import re           # 正規表現
import pprint
import sys

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
    GenerateTlmDef(settings, tlm_db)


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
        comment = sgc_db[i][0]
        name    = sgc_db[i][1]
        cmd_id  = sgc_db[i][3]
        if comment == "" and name == "":                  # CommentもNameも空白なら打ち切り
            break
        if comment != "":                                 # Comment
            continue
        # print(name)
        cmd_name = name
        cmd_code = cmd_name.replace("Cmd_", "Cmd_CODE_")
        # print(cmd_name)
        # print(cmd_code)
        body_c += "  cmd_table_[" + cmd_code + "].cmd_func = " + cmd_name + ";\n"
        body_h += "  " + cmd_code + " = " + cmd_id + ",\n"

    OutputCmdDefC(output_file_path + output_file_name_base + ".c", body_c)
    OutputCmdDefH(output_file_path + output_file_name_base + ".h", body_h)


def GenerateBctDef(settings, bct_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/CmdTlm/"
    output_file_name = "BlockCommandDefinitions.h"

    DATA_SART_ROW = 2

    body_h = ""
    # "  cmd_table_[Cmd_CODE_NOP].cmd_func = Cmd_NOP;"
    # "  Cmd_CODE_NOP = 0x0000,"
    for i in range(DATA_SART_ROW, len(bct_db)):
        comment = bct_db[i][0]
        name    = bct_db[i][1]
        bc_id   = bct_db[i][3]

        if comment == "" and name == "":                    # CommentもNameも空白なら打ち切り
            break

        if comment == "**":                                 # New Line Comment
            body_h += "\n  // " + name + "\n"
        elif comment != "":                                 # Comment
            body_h += "  // " + name + "\n"
        else:
            # "  BC_SL_INITIAL_TO_INITIAL = 0,"
            body_h += "  " + name + " = " + bc_id +",\n"

    OutputBctDef(output_file_path + output_file_name, body_h)


def GenerateTlmDef(settings, tlm_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/CmdTlm/"
    output_file_name_base = "TelemetryDefinitions"

    DATA_SART_ROW = 7

    body_c = ""
    body_h = ""

    # "static int OBC_(unsigned char* contents, int max_len);"
    # "  OBC_ID = 0x00,"
    for tlm in tlm_db:
        body_c += "static int " + tlm['tlm_name'] + "_(unsigned char* contents, int max_len);\n"
        body_h += "  " + tlm['tlm_name'] + "_ID = " + tlm['tlm_id'] + ",\n"

    body_c += "\n"
    body_c += "void TF_load_tlm_table(TlmInfo tlm_table_[TLM_MAX_TLMS])\n"
    body_c += "{\n"
    for tlm in tlm_db:
        # "  tlm_table_[OBC_ID].tlm_func = OBC_;"
        body_c += "  tlm_table_[" + tlm['tlm_name'] + "_ID].tlm_func = " + tlm['tlm_name'] + "_;\n"
    body_c += "}\n"

    for tlm in tlm_db:
        func_code = ""
        max_pos = ""
        for i in range(DATA_SART_ROW, len(tlm['data'])):
            comment  = tlm['data'][i][0]
            name     = tlm['data'][i][1]
            var_type = tlm['data'][i][2]
            code     = tlm['data'][i][3]
            pos      = tlm['data'][i][4]
            if comment == "" and name == "":                    # CommentもNameも空白なら打ち切り
                break
            if comment != "":
                continue
            if name == "":
                continue
            if var_type == "":
                continue
            if code == "":
                continue
            if pos == "":
                continue

            code = code.replace("@@", ",")
            func_code += "  "
            if   var_type == "int8_t":
                func_code += "copy_i8_"
                max_pos = int(pos) + 1
            elif var_type == "int16_t":
                func_code += "copy_i16_"
                max_pos = int(pos) + 2
            elif var_type == "int32_t":
                func_code += "copy_i32_"
                max_pos = int(pos) + 4
            elif var_type == "uint8_t":
                func_code += "copy_u8_"
                max_pos = int(pos) + 1
            elif var_type == "uint16_t":
                func_code += "copy_u16_"
                max_pos = int(pos) + 2
            elif var_type == "uint32_t":
                func_code += "copy_u32_"
                max_pos = int(pos) + 4
            elif var_type == "float":
                func_code += "copy_float_"
                max_pos = int(pos) + 4
            elif var_type == "double":
                func_code += "copy_double_"
                max_pos = int(pos) + 8
            else:
                print("Error: Tlm DB Err at " + tlm['tlm_name'], file=sys.stderr)
                sys.exit(1)
            func_code += "(&contents[" + pos + "], " + code + ");\n"

        body_c += "\n"
        body_c += "static int " + tlm['tlm_name'] + "_(unsigned char* contents, int max_len)\n"
        body_c += "{\n"
        body_c += "\n"
        body_c += "  if( " + str(max_pos) + " > max_len) { return TLM_TOO_SHORT_LEN; }\n"
        body_c += "\n"
        body_c += func_code
        body_c += "\n"
        body_c += "  return " + str(max_pos) + ";\n"
        body_c += "}\n"

    OutputTlmDefC(output_file_path + output_file_name_base + ".c", body_c)
    OutputTlmDefH(output_file_path + output_file_name_base + ".h", body_h)


def OutputCmdDefC(file_path, body):
    output = ""
    output += '''
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

    output += body

    output += '''
}

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/

#pragma section
'''[1:]         # 最初の改行を除く


    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputCmdDefH(file_path, body):
    output = ""
    output += '''
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

    output += body

    output += '''

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

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputBctDef(file_path, body):
    output = ""
    output += '''
/**
 * @file   BlockCommandDefinitions.h
 * @brief  ブロックコマンド定義
 * @author 鈴本 遼
 * @date   2020/11/14
 */
#ifndef BLOCK_COMMAND_DEFINITIONS_H_
#define BLOCK_COMMAND_DEFINITIONS_H_

// 登録されるBlockCommandTableのblock番号を規定
typedef enum
{
'''[1:]         # 最初の改行を除く

    output += body

    output += '''
} BC_DEFAULTS;

void BC_load_defaults(void);

#endif // BLOCK_COMMAND_DEFINISIONS_H_
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputTlmDefC(file_path, body):
    output = ""
    output += '''
#pragma section REPRO
/**
 * @file   TelemetryDefinitions.c
 * @brief  テレメトリ定義
 * @author 鈴本 遼
 * @date   2020/08/23
 */
#include "TelemetryDefinitions.h"
#include "TelemetrySource.h"

/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
'''[1:]         # 最初の改行を除く

    output += body

    output += '''

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/

#pragma section
'''[1:]         # 最初の改行を除く


    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputTlmDefH(file_path, body):
    output = ""
    output += '''
/**
 * @file   TelemetryDefinitions.h
 * @brief  テレメトリ定義
 * @author 鈴本 遼
 * @date   2020/08/23
 */
#ifndef TELEMETRY_DEFINITIONS_H_
#define TELEMETRY_DEFINITIONS_H_

#include "../../src_core/CmdTlm/TelemetryFrame.h"

typedef enum
{
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##

'''[1:]         # 最初の改行を除く

    output += body

    output += '''

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
  TLM_ID_MAX
} TLMID;

void TF_load_tlm_table(TlmInfo tlm_table_[TLM_MAX_TLMS]);


#endif // TLM_FRAME_INCLUDE
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


if __name__ == '__main__':
    main()

