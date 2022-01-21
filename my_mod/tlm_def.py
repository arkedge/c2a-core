# coding: UTF-8
"""
tlm def
"""

import sys


def GenerateTlmDef(settings, tlm_db, other_obc_dbs):
    output_file_path = settings["c2a_root_dir"] + r"src_user/TlmCmd/"
    output_file_name_base = "telemetry_definitions"

    DATA_START_ROW = 8

    body_c = ""
    body_h = ""

    # "static int OBC_(unsigned char* contents, int max_len);"
    # "  OBC_ID = 0x00,"
    for tlm in tlm_db:
        body_c += (
            "static int Tlm_"
            + tlm["tlm_name"].upper()
            + "_(unsigned char* contents, int max_len);\n"
        )
        body_h += "  Tlm_CODE_" + tlm["tlm_name"].upper() + " = " + tlm["tlm_id"] + ",\n"

    if settings["is_main_obc"]:
        body_c += GetTlmDefCOfOtherObcFunDef_(settings, tlm_db, other_obc_dbs)

    body_c += "\n"
    body_c += "void TF_load_tlm_table(TF_TlmInfo tlm_table[TF_MAX_TLMS])\n"
    body_c += "{\n"
    for tlm in tlm_db:
        # "  tlm_table[OBC_ID].tlm_func = OBC_;"
        body_c += (
            "  tlm_table[Tlm_CODE_"
            + tlm["tlm_name"].upper()
            + "].tlm_func = Tlm_"
            + tlm["tlm_name"].upper()
            + "_;\n"
        )
    if settings["is_main_obc"]:
        body_c += GetTlmDefCOfOtherObcFunLoad_(settings, tlm_db, other_obc_dbs)
    body_c += "}\n"

    for tlm in tlm_db:
        func_code = ""
        max_pos = ""
        for i in range(DATA_START_ROW, len(tlm["data"])):
            comment = tlm["data"][i][0]
            name = tlm["data"][i][1]
            var_type = tlm["data"][i][2]
            code = tlm["data"][i][3]
            pos = tlm["data"][i][5]
            if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
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

            pos = int(pos) - settings["header_len"]
            code = code.replace("@@", ",")
            func_code += "  "
            if var_type == "int8_t":
                func_code += "TF_copy_i8"
                max_pos = pos + 1
            elif var_type == "int16_t":
                func_code += "TF_copy_i16"
                max_pos = pos + 2
            elif var_type == "int32_t":
                func_code += "TF_copy_i32"
                max_pos = pos + 4
            elif var_type == "uint8_t":
                func_code += "TF_copy_u8"
                max_pos = pos + 1
            elif var_type == "uint16_t":
                func_code += "TF_copy_u16"
                max_pos = pos + 2
            elif var_type == "uint32_t":
                func_code += "TF_copy_u32"
                max_pos = pos + 4
            elif var_type == "float":
                func_code += "TF_copy_float"
                max_pos = pos + 4
            elif var_type == "double":
                func_code += "TF_copy_double"
                max_pos = pos + 8
            else:
                print("Error: Tlm DB Err at " + tlm["tlm_name"].upper(), file=sys.stderr)
                sys.exit(1)
            func_code += "(&contents[" + str(pos) + "], " + code + ");\n"

        body_c += "\n"
        body_c += (
            "static int Tlm_"
            + tlm["tlm_name"].upper()
            + "_(unsigned char* contents, int max_len)\n"
        )
        body_c += "{\n"
        for local_var in tlm["local_vars"]:
            body_c += "  " + local_var + "\n"
        if len(tlm["local_vars"]) > 0:
            body_c += "\n"
        body_c += "  if (" + str(max_pos) + " > max_len) return TF_TOO_SHORT_LEN;\n"
        body_c += "\n"
        body_c += "#ifndef BUILD_SETTINGS_FAST_BUILD\n"
        body_c += func_code
        body_c += "#endif\n"
        body_c += "\n"
        body_c += "  return " + str(max_pos) + ";\n"
        body_c += "}\n"

    if settings["is_main_obc"]:
        body_c += GetTlmDefCOfOtherObcFunBody_(settings, tlm_db, other_obc_dbs)
        body_h += GetTlmDefHOfOtherObc_(settings, tlm_db, other_obc_dbs)

    OutputTlmDefC_(output_file_path + output_file_name_base + ".c", body_c, settings)
    OutputTlmDefH_(output_file_path + output_file_name_base + ".h", body_h, settings)


def GetTlmDefCOfOtherObcFunDef_(settings, tlm_db, other_obc_dbs):
    body_c = ""

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue

        obc_name = settings["other_obc_data"][i]["name"]
        oter_obc_tlm_db = other_obc_dbs[obc_name]

        temp_c = ""
        temp_c += "\n"
        temp_c += "// {_obc_name_upper} TLM\n"
        for tlm in oter_obc_tlm_db:
            temp_c += (
                "static int Tlm_"
                + tlm["tlm_name"].upper()
                + "_(unsigned char* contents, int max_len);\n"
            )

        body_c += temp_c.format(
            _obc_name_upper=obc_name.upper(),
            _obc_name_lower=obc_name.lower(),
            _obc_name_capit=obc_name.capitalize(),
        )

    return body_c


def GetTlmDefCOfOtherObcFunLoad_(settings, tlm_db, other_obc_dbs):
    body_c = ""

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue

        obc_name = settings["other_obc_data"][i]["name"]
        oter_obc_tlm_db = other_obc_dbs[obc_name]

        temp_c = ""
        temp_c += "\n"
        temp_c += "  // {_obc_name_upper} TLM\n"
        for tlm in oter_obc_tlm_db:
            temp_c += (
                "  tlm_table[Tlm_CODE_"
                + tlm["tlm_name"].upper()
                + "].tlm_func = Tlm_"
                + tlm["tlm_name"].upper()
                + "_;\n"
            )
        body_c += temp_c.format(
            _obc_name_upper=obc_name.upper(),
            _obc_name_lower=obc_name.lower(),
            _obc_name_capit=obc_name.capitalize(),
        )

    return body_c


def GetTlmDefCOfOtherObcFunBody_(settings, tlm_db, other_obc_dbs):
    body_c = ""

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue

        obc_name = settings["other_obc_data"][i]["name"]
        oter_obc_tlm_db = other_obc_dbs[obc_name]

        temp_c = ""
        for tlm in oter_obc_tlm_db:
            tlm_name = tlm["tlm_name"]
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            temp_c += "\n"
            temp_c += (
                "static int Tlm_" + tlm_name_upper + "_(unsigned char* contents, int max_len)\n"
            )
            temp_c += "{{\n"
            temp_c += "  int buffer_size = {_obc_name_lower}_buffer->" + tlm_name_lower + ".size;\n"
            temp_c += "\n"
            temp_c += "  if (buffer_size > max_len) {{ return TF_TOO_SHORT_LEN; }}\n"
            temp_c += "\n"
            temp_c += (
                "  memcpy(contents, {_obc_name_lower}_buffer->"
                + tlm_name_lower
                + ".buffer, (size_t)buffer_size);\n"
            )
            temp_c += "\n"
            temp_c += "  return buffer_size;\n"
            temp_c += "}}\n"

        body_c += temp_c.format(
            _obc_name_upper=obc_name.upper(),
            _obc_name_lower=obc_name.lower(),
            _obc_name_capit=obc_name.capitalize(),
        )

    return body_c


def GetTlmDefHOfOtherObc_(settings, tlm_db, other_obc_dbs):
    # ID重複チェックはここでやり，他のGet関数ではやらない（実装イマイチ．．．）
    body_h = ""

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]

        temp_h = ""

        id_begin = int(settings["other_obc_data"][i]["tlm_id_range"][0], 0)
        id_end = int(settings["other_obc_data"][i]["tlm_id_range"][1], 0)

        # MOBC Tlmが範囲外かチェック
        for tlm in tlm_db:
            tlm_id = int(tlm["tlm_id"], 0)
            if id_begin <= tlm_id < id_end:
                print(
                    "Error: MOBC TLM ID is invalid at " + tlm["tlm_name"].upper(), file=sys.stderr
                )
                sys.exit(1)

        oter_obc_tlm_db = other_obc_dbs[obc_name]
        # ID範囲内チェック
        for tlm in oter_obc_tlm_db:
            tlm_id = int(tlm["tlm_id"], 0)
            if not id_begin <= tlm_id < id_end:
                print(
                    "Error: " + obc_name + " TLM ID is invalid at " + tlm["tlm_name"].upper(),
                    file=sys.stderr,
                )
                sys.exit(1)

        body_h += "\n"
        temp_h += "  // {_obc_name_upper} TLM\n"
        for tlm in oter_obc_tlm_db:
            temp_h += "  Tlm_CODE_" + tlm["tlm_name"].upper() + " = " + tlm["tlm_id"] + ",\n"

        body_h += temp_h.format(
            _obc_name_upper=obc_name.upper(),
            _obc_name_lower=obc_name.lower(),
            _obc_name_capit=obc_name.capitalize(),
        )

    body_h += "\n"
    body_h += "  // FIXME:\n"
    body_h += "  // 他OBCのTLMをまとめたため，IDが昇順となっていない\n"
    body_h += "  // TLM_CODE_MAX が意味をなさなくなってしまうが，他で使われていないため，一旦このままにする（今後解決する実装）\n"

    return body_h


def GenerateOtherObcTlmDef(settings, other_obc_dbs):
    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]

        tlm_db = other_obc_dbs[obc_name]

        body_h = ""
        # "  TOBC_Tlm_CODE_HK = 0xf0,"
        for tlm in tlm_db:
            body_h += (
                "  {_obc_name_upper}_Tlm_CODE_"
                + tlm["tlm_name"].upper()
                + " = "
                + tlm["tlm_id"]
                + ",\n"
            )
        output_file_path = (
            settings["c2a_root_dir"]
            + r"src_user/Drivers/"
            + settings["other_obc_data"][i]["driver_path"]
            + obc_name.lower()
            + "_telemetry_definitions.h"
        )
        OutputOtherObcTlmDefH(output_file_path, obc_name, body_h, settings)


def OutputTlmDefC_(file_path, body, settings):
    output = ""
    output += """
#pragma section REPRO
/**
 * @file
 * @brief  テレメトリ定義
 * @note   このコードは自動生成されています！
 */
#include "../../src_core/TlmCmd/telemetry_frame.h"
#include "telemetry_definitions.h"
#include "telemetry_source.h"

"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

#pragma section
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(output)


def OutputTlmDefH_(file_path, body, settings):
    output = ""
    output += """
/**
 * @file
 * @brief  テレメトリ定義
 * @note   このコードは自動生成されています！
 */
#ifndef TELEMETRY_DEFINITIONS_H_
#define TELEMETRY_DEFINITIONS_H_

typedef enum
{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

  TLM_CODE_MAX
} TLM_CODE;

#endif
"""[
        1:
    ]  # 最初の改行を除く

    with open(file_path, mode="w", encoding=settings["output_file_encoding"]) as fh:
        fh.write(output)


def OutputOtherObcTlmDefH(file_path, name, body, settings):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += """
/**
 * @file
 * @brief  テレメトリ定義
 * @note   このコードは自動生成されています！
 */
#ifndef {_obc_name_upper}_TELEMETRY_DEFINITIONS_H_
#define {_obc_name_upper}_TELEMETRY_DEFINITIONS_H_

typedef enum
{{
"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

  {_obc_name_upper}_TLM_CODE_MAX
}} {_obc_name_upper}_TLM_CODE;

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
