# coding: UTF-8
"""
tlm buffer
"""

import os

def GenerateTlmBuffer(settings, other_obc_dbs):
    DATA_SART_ROW = 7

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]
        driver_type = settings["other_obc_data"][i]["driver_type"]
        driver_name = settings["other_obc_data"][i]["driver_name"]
        contents_len = settings["other_obc_data"][i]["tlm_max_contents_len"]

        tlm_db = other_obc_dbs[obc_name]

        body_c = ""
        body_h = ""

        body_c += "static {_obc_name_upper}_Buffer {_obc_name_lower}_buffer_;\n"
        body_c += "const {_obc_name_upper}_Buffer* {_obc_name_lower}_buffer;\n"
        body_c += "\n"
        body_h += "typedef struct {{\n"
        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_h += "  uint8_t " + tlm_name_lower + "[" + str(contents_len) + "];\n"
            body_c += "static DRIVER_SUPER_ERR_CODE {_obc_name_upper}_analyze_tlm_" + tlm_name_lower + "_(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ");\n"

        body_h += "}} {_obc_name_upper}_Buffer;\n"
        body_h += "\n"
        body_h += "extern const {_obc_name_upper}_Buffer* {_obc_name_lower}_buffer;\n"
        body_h += "\n"
        body_h += "DRIVER_SUPER_ERR_CODE {_obc_name_upper}_buffer_init(void);\n"
        body_h += "DRIVER_SUPER_ERR_CODE {_obc_name_upper}_buffer_tlm_contents(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ");\n"


        body_c += "\n"
        body_c += "DRIVER_SUPER_ERR_CODE {_obc_name_upper}_buffer_init(void)\n"
        body_c += "{{\n"
        body_c += "  {_obc_name_lower}_buffer = &{_obc_name_lower}_buffer_;\n"
        body_c += "}}\n"
        body_c += "\n"
        body_c += "DRIVER_SUPER_ERR_CODE {_obc_name_upper}_buffer_tlm_contents(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ")\n"
        body_c += "{{\n"

        body_c += "  uint8_t tlm_id  = DRIVER_SUPER_C2AFMT_get_tlm_id(p_stream_config);\n"
        body_c += "\n"

        body_c += "  switch (tlm_id)\n"
        body_c += "  {{\n"
        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_c += "  case {_obc_name_upper}_Tlm_CODE_" + tlm_name_upper + ":\n"
            body_c += "    return {_obc_name_upper}_analyze_tlm_" + tlm_name_lower + "_(p_stream_config, " + driver_name + ")\n"
        body_c += "  default:\n"
        body_c += "    " + settings["other_obc_data"][i]["code_when_tlm_not_found"] + "\n"
        body_c += "    return DRIVER_SUPER_ERR_CODE_OK;\n"
        body_c += "  }}\n"
        body_c += "}}\n"
        body_c += "\n"
        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_c += "static DRIVER_SUPER_ERR_CODE {_obc_name_upper}_analyze_tlm_" + tlm_name_lower + "_(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ")\n"
            body_c += "{{\n"
            body_c += "  uint32_t tlm_len = DRIVER_SUPER_ISSLFMT_get_tlm_length(p_stream_config);\n"
            body_c += "  uint32_t contents_len = tlm_len - DRIVER_SUPER_C2AFMT_TCP_TLM_SECONDARY_HEADER_SIZE - 1;      // FIXME: CCSDSは1起算？\n"
            body_c += "  uint8_t* contents_pos = (p_stream_config->rx_frame) + DRIVER_SUPER_ISSLFMT_COMMON_HEADER_SIZE + DRIVER_SUPER_C2AFMT_TCP_TLM_PRIMARY_HEADER_SIZE + DRIVER_SUPER_C2AFMT_TCP_TLM_SECONDARY_HEADER_SIZE;\n"
            body_c += "\n"
            body_c += "  if (contents_len > " + str(contents_len) + ") return DRIVER_SUPER_ERR_CODE_ERR;\n"
            body_c += "  memcpy(&({_obc_name_lower}_buffer_" + tlm_name_lower + "), contents_pos, (size_t)contents_len);\n"
            body_c += "\n"
            body_c += "  // [TODO] フレームの中身をパースしてMOBCでもろもろにアクセスするためのコード\n"
            body_c += "  //        テレメDBと同じ構造の構造体を定義して代入する？\n"
            body_c += "  //        問題はビットフィールドを使ってるやつら\n"
            body_c += "  return DRIVER_SUPER_ERR_CODE_OK;\n"
            body_c += "}}\n"
            body_c += "\n"


        output_file_path = settings["c2a_root_dir"] + r"src_user/Drivers/" + settings["other_obc_data"][i]["driver_path"];
        OutputTlmBufferC(output_file_path + obc_name.capitalize() + "TelemetryBuffer.c", obc_name, body_c)
        OutputTlmBufferH(output_file_path + obc_name.capitalize() + "TelemetryBuffer.h", obc_name, body_h)


def OutputTlmBufferC(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
#pragma section REPRO
/**
 * @file   {_obc_name_capit}TelemetryBuffer.c
 * @brief  テレメトリバッファー（テレメ中継）
 * @note   このコードは自動生成されています！
 * @author 鈴本 遼
 * @date   2021/02/14
 */

#include "./{_obc_name_capit}TelemetryDefinitions.h"
#include "./{_obc_name_capit}TelemetryBuffer.h"

'''[1:]         # 最初の改行を除く

    output += body

    output += '''
#pragma section
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output.format(_obc_name_upper=name_upper, _obc_name_lower=name_lower, _obc_name_capit=name_capit))





def OutputTlmBufferH(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
/**
 * @file   {_obc_name_capit}TelemetryBuffer.h
 * @brief  テレメトリバッファー（テレメ中継）
 * @note   このコードは自動生成されています！
 * @author 鈴本 遼
 * @date   2021/02/14
 */
#ifndef {_obc_name_upper}_TELEMETRY_BUFFER_H_
#define {_obc_name_upper}_TELEMETRY_BUFFER_H_

#include "string.h" // for memcpy
#include "./{_obc_name_upper}.h"

'''[1:]         # 最初の改行を除く

    output += body

    output += '''

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output.format(_obc_name_upper=name_upper, _obc_name_lower=name_lower, _obc_name_capit=name_capit))

