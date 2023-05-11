# coding: UTF-8
"""
cmd def
"""

import sys

# import pprint


def GenerateCmdDef(settings, sgc_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/TlmCmd/"
    output_file_name_base = "command_definitions"

    DATA_SART_ROW = 3

    body_c = ""
    body_h = ""
    # "  cmd_table[Cmd_CODE_NOP].cmd_func = Cmd_NOP;"
    # "  Cmd_CODE_NOP = 0x0000,"
    for i in range(DATA_SART_ROW, len(sgc_db)):
        comment = sgc_db[i][0]
        name = sgc_db[i][1]
        cmd_id = sgc_db[i][3]
        if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
            break
        if comment != "":  # Comment
            continue

        cmd_name, cmd_code = GetCmdNameAndCmdCode_(name, settings["is_cmd_prefixed_in_db"])
        # print(cmd_name)
        # print(cmd_code)
        body_c += "  cmd_table[" + cmd_code + "].cmd_func = " + cmd_name + ";\n"
        body_h += "  " + cmd_code + " = " + cmd_id + ",\n"

    body_c += "\n"
    for i in range(DATA_SART_ROW, len(sgc_db)):
        comment = sgc_db[i][0]
        name = sgc_db[i][1]
        cmd_id = sgc_db[i][3]
        if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
            break
        if comment != "":  # Comment
            continue

        param_num = int(sgc_db[i][4])
        type_list = [
            sgc_db[i][5],
            sgc_db[i][7],
            sgc_db[i][9],
            sgc_db[i][11],
            sgc_db[i][13],
            sgc_db[i][15],
        ]
        cmd_name, cmd_code = GetCmdNameAndCmdCode_(name, settings["is_cmd_prefixed_in_db"])

        # パラメタ長の整合性チェック
        for j in range(len(type_list)):
            err_flag = 0
            if j < param_num and type_list[j] == "":
                err_flag = 1
            if j >= param_num and type_list[j] != "":
                err_flag = 1
            if err_flag:
                print("Error: Cmd DB Err at " + name, file=sys.stderr)
                sys.exit(1)

        # パラメタ長のカウント
        conv_tpye_to_size = {
            "int8_t": "CA_PARAM_SIZE_TYPE_1BYTE",
            "int16_t": "CA_PARAM_SIZE_TYPE_2BYTE",
            "int32_t": "CA_PARAM_SIZE_TYPE_4BYTE",
            "uint8_t": "CA_PARAM_SIZE_TYPE_1BYTE",
            "uint16_t": "CA_PARAM_SIZE_TYPE_2BYTE",
            "uint32_t": "CA_PARAM_SIZE_TYPE_4BYTE",
            "float": "CA_PARAM_SIZE_TYPE_4BYTE",
            "double": "CA_PARAM_SIZE_TYPE_8BYTE",
            "raw": "CA_PARAM_SIZE_TYPE_RAW",
        }
        for j in range(param_num):
            index = j // 2
            subindex = "second" if j % 2 else "first"
            body_c += (
                "  cmd_table["
                + cmd_code
                + "].param_size_infos["
                + str(index)
                + "].packed_info.bit."
                + subindex
                + " = "
                + conv_tpye_to_size[type_list[j]]
                + ";\n"
            )

    OutputCmdDefC_(output_file_path + output_file_name_base + ".c", body_c, settings)
    OutputCmdDefH_(output_file_path + output_file_name_base + ".h", body_h, settings)


def GenerateBctDef(settings, bct_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/TlmCmd/"
    output_file_name = "block_command_definitions.h"

    DATA_SART_ROW = 2

    body_h = ""
    for i in range(DATA_SART_ROW, len(bct_db)):
        comment = bct_db[i][0]
        name = bct_db[i][1]
        bc_id = bct_db[i][3]
        description = bct_db[i][10]

        # エスケープ解除
        name = name.replace("@@", ",")
        description = description.replace("@@", ",")

        if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
            break

        if comment == "**":  # New Line Comment
            body_h += "\n  // " + name + "\n"
        elif comment != "":  # Comment
            body_h += "  // " + name + "\n"
        else:
            # "  BC_SL_INITIAL_TO_INITIAL = 0,"
            if description == "":
                body_h += "  " + name + " = " + bc_id + ",\n"
            else:
                body_h += "  " + name + " = " + bc_id + ",    // " + description + "\n"

    OutputBctDef_(output_file_path + output_file_name, body_h, settings)


def GenerateOtherObcCmdDef(settings, other_obc_dbs):
    # pprint.pprint(other_obc_dbs)
    DATA_SART_ROW = 3
    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]
        name_upper = obc_name.upper()
        name_lower = obc_name.lower()
        # name_capit = obc_name.capitalize()
        # print(name_upper)
        # print(name_lower)
        # print(name_capit)
        sgc_db = other_obc_dbs[obc_name]
        # pprint.pprint(sgc_db)

        body_h = ""
        # "  TOBC_Cmd_CODE_NOP = 0x0000,"
        for j in range(DATA_SART_ROW, len(sgc_db)):
            comment = sgc_db[j][0]
            name = sgc_db[j][1]
            cmd_id = sgc_db[j][3]
            if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
                break
            if comment != "":  # Comment
                continue
            # print(name)
            _, cmd_code = GetCmdNameAndCmdCode_(
                name, settings["other_obc_data"][i]["is_cmd_prefixed_in_db"]
            )
            cmd_code = name_upper + "_" + cmd_code
            body_h += "  " + cmd_code + " = " + cmd_id + ",\n"
        # print(body_h)
        output_file_path = (
            settings["c2a_root_dir"]
            + r"src_user/Drivers/"
            + settings["other_obc_data"][i]["driver_path"]
            + name_lower
            + "_command_definitions.h"
        )
        OutputOtherObcCmdDefH_(output_file_path, obc_name, body_h, settings)


def GetCmdNameAndCmdCode_(name, is_cmd_prefixed_in_db):
    if is_cmd_prefixed_in_db:
        cmd_name = name
    else:
        cmd_name = "Cmd_" + name
    cmd_code = cmd_name.replace("Cmd_", "Cmd_CODE_")
    return cmd_name, cmd_code


def OutputCmdDefC_(file_path, body, settings):
    output = ""
    output += """
#pragma section REPRO
/**
 * @file
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
 */
#include <src_core/TlmCmd/command_analyze.h>
#include "command_definitions.h"
#include "command_source.h"

void CA_load_cmd_table(CA_CmdInfo cmd_table[CA_MAX_CMDS])
{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """
}

#pragma section
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(output)


def OutputCmdDefH_(file_path, body, settings):
    output = ""
    output += """
/**
 * @file
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
 */
#ifndef COMMAND_DEFINITIONS_H_
#define COMMAND_DEFINITIONS_H_

typedef enum
{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

  Cmd_CODE_MAX
} CMD_CODE;

#endif
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(output)


def OutputBctDef_(file_path, body, settings):
    output = ""
    output += """
/**
 * @file
 * @brief  ブロックコマンド定義
 * @note   このコードは自動生成されています！
 */
#ifndef BLOCK_COMMAND_DEFINITIONS_H_
#define BLOCK_COMMAND_DEFINITIONS_H_

// 登録されるBlockCommandTableのblock番号を規定
typedef enum
{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

  BC_ID_MAX    // BCT 自体のサイズは BCT_MAX_BLOCKS で規定
} BC_DEFAULT_ID;

void BC_load_defaults(void);

#endif
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(output)


def OutputOtherObcCmdDefH_(file_path, name, body, settings):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += """
/**
 * @file
 * @brief  コマンド定義
 * @note   このコードは自動生成されています！
 */
#ifndef {_obc_name_upper}_COMMAND_DEFINITIONS_H_
#define {_obc_name_upper}_COMMAND_DEFINITIONS_H_

typedef enum
{{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

  {_obc_name_upper}_Cmd_CODE_MAX
}} {_obc_name_upper}_CMD_CODE;

#endif
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(
            output.format(
                _obc_name_upper=name_upper, _obc_name_lower=name_lower, _obc_name_capit=name_capit
            )
        )
