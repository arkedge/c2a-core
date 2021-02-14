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
        name_upper = obc_name.upper()
        name_lower = obc_name.lower()
        name_capit = obc_name.capitalize()
        driver_type = settings["other_obc_data"][i]["driver_type"]
        driver_name = settings["other_obc_data"][i]["driver_name"]
        contents_len = settings["other_obc_data"][i]["tlm_max_contents_len"]

        tlm_db = other_obc_dbs[obc_name]

        body_c = ""
        body_h = ""

        body_h += "typedef struct {\n"

        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_h += "  uint8_t " + tlm_name_lower + "[" + str(contents_len) + "];\n"
            body_c += "static DRIVER_SUPER_ERR_CODE " + name_upper + "_analyze_tlm_" + tlm_name_lower + "_(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ");\n"

        body_h += "} " + name_upper + "_Buffer;\n"
        body_h += "\n"
        body_h += "extern const " + name_upper + "_Buffer* " + name_lower + "_buffer;\n"
        body_h += "\n"
        body_h += "DRIVER_SUPER_ERR_CODE " + name_upper + "_buffer_tlm_contents(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ");\n"


        body_c += "\n"
        body_c += "DRIVER_SUPER_ERR_CODE " + name_upper + "_buffer_tlm_contents(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ")\n"
        body_c += "{\n"
        body_c += "  switch (tlm_id)\n"
        body_c += "  {\n"
        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_c += "  case " + name_upper + "_Tlm_CODE_" + tlm_name_upper + ":\n"
            body_c += "    return " + name_upper + "_analyze_tlm_" + tlm_name_lower + "_(p_stream_config, " + driver_name + ")\n"
        body_c += "  default:\n"
        body_c += "    " + settings["other_obc_data"][i]["code_when_tlm_not_found"] + "\n"
        body_c += "    return DRIVER_SUPER_ERR_CODE_OK;\n"
        body_c += "  }\n"
        body_c += "}\n"
        body_c += "\n"
        for tlm in tlm_db:
            tlm_name = tlm['tlm_name']
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_c += "static DRIVER_SUPER_ERR_CODE " + name_upper + "_analyze_tlm_" + tlm_name_lower + "_(DriverSuperStreamConfig *p_stream_config, " + driver_type + " *" + driver_name + ")\n"
            body_c += "{\n"
            body_c += "  // [TODO] フレームの中身をパースしてMOBCでもろもろにアクセスするためのコード\n"
            body_c += "  return DRIVER_SUPER_ERR_CODE_OK;\n"
            body_c += "}\n"
            body_c += "\n"


        output_file_path = settings["c2a_root_dir"] + r"src_user/Drivers/" + settings["other_obc_data"][i]["driver_path"];
        OutputTlmBufferC(output_file_path + name_capit + "TelemetryBuffer.c", obc_name, body_c)
        OutputTlmBufferH(output_file_path + name_capit + "TelemetryBuffer.h", obc_name, body_h)


def OutputTlmBufferC(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
#pragma section REPRO
/**
'''[1:]         # 最初の改行を除く

    output += " * @file   " + name_capit + "TelemetryBuffer.c\n"

    output += '''
 * @brief  テレメトリバッファー（テレメ中継）
 * @author 鈴本 遼
 * @date   2021/02/14
 */
'''[1:]         # 最初の改行を除く

    output += "\n"
    output += '#include "./' + name_capit + 'TelemetryDefinitions.h"\n'
    output += '#include "./' + name_capit + 'TelemetryBuffer.h"\n'
    output += "\n"

    output += body

    output += '''

#pragma section
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)


def OutputTlmBufferH(file_path, name, body):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += '''
/**
'''[1:]         # 最初の改行を除く

    output += " * @file   " + name_capit + "TelemetryBuffer.h\n"

    output += '''
 * @brief  テレメトリバッファー（テレメ中継）
 * @author 鈴本 遼
 * @date   2021/02/14
 */
'''[1:]         # 最初の改行を除く

    output += "#ifndef " + name_upper + "_TELEMETRY_BUFFER_H_\n"
    output += "#define " + name_upper + "_TELEMETRY_BUFFER_H_\n"
    output += "\n"
    output += '#include "./' + name_upper + '.h"\n'

    output += "\n"

    output += body

    output += '''

#endif
'''[1:]         # 最初の改行を除く

    with open(file_path, mode='w', encoding='shift_jis') as fh:
        fh.write(output)
