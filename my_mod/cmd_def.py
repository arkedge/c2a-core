# coding: UTF-8
"""
cmd def
"""

import os

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

    OutputBctDef(output_file_path + output_file_name, body_h)


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
  Cmd_CODE_MAX
} CMD_CODE;

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
} BC_DEFAULT_ID;

void BC_load_defaults(void);

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)

