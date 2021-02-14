# coding: UTF-8
"""
tlm def
"""

import os

def GenerateTlmDef(settings, tlm_db):
    output_file_path = settings["c2a_root_dir"] + r"src_user/CmdTlm/"
    output_file_name_base = "TelemetryDefinitions"

    DATA_SART_ROW = 7

    body_c = ""
    body_h = ""

    # "static int OBC_(unsigned char* contents, int max_len);"
    # "  OBC_ID = 0x00,"
    for tlm in tlm_db:
        body_c += "static int Tlm_" + tlm['tlm_name'] + "_(unsigned char* contents, int max_len);\n"
        body_h += "  Tlm_CODE_" + tlm['tlm_name'] + " = " + tlm['tlm_id'] + ",\n"

    body_c += "\n"
    body_c += "void TF_load_tlm_table(TlmInfo tlm_table_[TLM_MAX_TLMS])\n"
    body_c += "{\n"
    for tlm in tlm_db:
        # "  tlm_table_[OBC_ID].tlm_func = OBC_;"
        body_c += "  tlm_table_[Tlm_CODE_" + tlm['tlm_name'] + "].tlm_func = Tlm_" + tlm['tlm_name'] + "_;\n"
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
        body_c += "static int Tlm_" + tlm['tlm_name'] + "_(unsigned char* contents, int max_len)\n"
        body_c += "{\n"
        body_c += "\n"
        body_c += "  if (" + str(max_pos) + " > max_len) { return TLM_TOO_SHORT_LEN; }\n"
        body_c += "\n"
        body_c += "#ifndef FAST_BUILD\n"
        body_c += func_code
        body_c += "#endif\n"
        body_c += "\n"
        body_c += "  return " + str(max_pos) + ";\n"
        body_c += "}\n"

    OutputTlmDefC(output_file_path + output_file_name_base + ".c", body_c)
    OutputTlmDefH(output_file_path + output_file_name_base + ".h", body_h)


def GenerateOtherObcTlmDef(settings, other_obc_dbs):
    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]
        name_upper = obc_name.upper()
        name_lower = obc_name.lower()
        name_capit = obc_name.capitalize()

        tlm_db = other_obc_dbs[obc_name]

        body_h = ""
        # "  TOBC_Tlm_CODE_HK = 0xf0,"
        for tlm in tlm_db:
            body_h += "  " + name_upper + "_Tlm_CODE_" + tlm['tlm_name'] + " = " + tlm['tlm_id'] + ",\n"
        output_file_path = settings["c2a_root_dir"] + r"src_user/Drivers/" + settings["other_obc_data"][i]["driver_path"] + name_capit + "TelemetryDefinitions.h"
        OutputOtherObcTlmDefH(output_file_path, obc_name, body_h)



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
  TLM_CODE_MAX
} TLM_CODE;

void TF_load_tlm_table(TlmInfo tlm_table_[TLM_MAX_TLMS]);

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputOtherObcTlmDefH(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
/**
'''[1:]         # 最初の改行を除く

    output += " * @file   " + name_capit + "TelemetryDefinitions.h\n"

    output += '''
 * @brief  テレメトリ定義
 * @author 鈴本 遼
 * @date   2020/08/23
 */
'''[1:]         # 最初の改行を除く

    output += "#ifndef " + name_upper + "_TELEMETRY_DEFINITIONS_H_\n"
    output += "#define " + name_upper + "_TELEMETRY_DEFINITIONS_H_\n"

    output += '''

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
'''[1:]         # 最初の改行を除く

    output += "  " + name_upper + "_TLM_CODE_MAX\n"
    output += "} " + name_upper + "_TLM_CODE;\n"

    output += '''

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)

