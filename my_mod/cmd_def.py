# coding: UTF-8
"""
cmd def
"""

import os
import pprint

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

    OutputCmdDefC_(output_file_path + output_file_name_base + ".c", body_c)
    OutputCmdDefH_(output_file_path + output_file_name_base + ".h", body_h)


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
        description = bct_db[i][10]

        if comment == "" and name == "":                    # CommentもNameも空白なら打ち切り
            break

        if comment == "**":                                 # New Line Comment
            body_h += "\n  // " + name + "\n"
        elif comment != "":                                 # Comment
            body_h += "  // " + name + "\n"
        else:
            # "  BC_SL_INITIAL_TO_INITIAL = 0,"
            if description == "":
                body_h += "  " + name + " = " + bc_id +",\n"
            else:
                body_h += "  " + name + " = " + bc_id +",    // " + description + "\n"

    OutputBctDef_(output_file_path + output_file_name, body_h)


def GenerateOtherObcCmdDef(settings, other_obc_dbs):
    # pprint.pprint(other_obc_dbs)
    DATA_SART_ROW = 3
    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]
        name_upper = obc_name.upper()
        name_lower = obc_name.lower()
        name_capit = obc_name.capitalize()
        # print(name_upper)
        # print(name_lower)
        # print(name_capit)
        sgc_db = other_obc_dbs[obc_name]
        # pprint.pprint(sgc_db)

        body_h = ""
        # "  TOBC_Cmd_CODE_NOP = 0x0000,"
        for j in range(DATA_SART_ROW, len(sgc_db)):
            comment = sgc_db[j][0]
            name    = sgc_db[j][1]
            cmd_id  = sgc_db[j][3]
            if comment == "" and name == "":                  # CommentもNameも空白なら打ち切り
                break
            if comment != "":                                 # Comment
                continue
            # print(name)
            cmd_name = name
            cmd_code = cmd_name.replace("Cmd_", name_upper+"_Cmd_CODE_")
            body_h += "  " + cmd_code + " = " + cmd_id + ",\n"
        # print(body_h)
        output_file_path = settings["c2a_root_dir"] + r"src_user/Drivers/" + settings["other_obc_data"][i]["driver_path"] + name_capit + "CommandDefinitions.h"
        OutputOtherObcCmdDefH_(output_file_path, obc_name, body_h)


def OutputCmdDefC_(file_path, body):
    output = ""
    output += '''
#pragma section REPRO
/**
 * @file   CommandDefinitions.c
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
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


def OutputCmdDefH_(file_path, body):
    output = ""
    output += '''
/**
 * @file   CommandDefinitions.h
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
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
  Cmd_CODE_MAX
} CMD_CODE;

void CA_load_cmd_table(CmdInfo cmd_table_[CMD_MAX_CMDS]);

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputBctDef_(file_path, body):
    output = ""
    output += '''
/**
 * @file   BlockCommandDefinitions.h
 * @brief  ブロックコマンド定義
 * @note   このコードは自動生成されています！
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
} BC_DEFAULT_ID;

void BC_load_defaults(void);

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputOtherObcCmdDefH_(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
/**
 * @file   {_obc_name_capit}CommandDefinitions.h
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
 * @author 鈴本 遼
 * @date   2020/08/23
 */
#ifndef {_obc_name_upper}_COMMAND_DEFINITIONS_H_
#define {_obc_name_upper}_COMMAND_DEFINITIONS_H_

typedef enum
{{
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
  {_obc_name_upper}_Cmd_CODE_MAX
}} {_obc_name_upper}_CMD_CODE;

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output.format(_obc_name_upper=name_upper, _obc_name_lower=name_lower, _obc_name_capit=name_capit))

