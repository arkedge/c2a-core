# coding: UTF-8
"""
tlm buffer
"""

import sys

# from collections import OrderedDict
# import pprint


def GenerateTlmBuffer(settings, other_obc_dbs):
    DATA_START_ROW = 8

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        obc_name = settings["other_obc_data"][i]["name"]
        driver_type = settings["other_obc_data"][i]["driver_type"]
        driver_name = settings["other_obc_data"][i]["driver_name"]
        max_tlm_num = settings["other_obc_data"][i]["max_tlm_num"]

        tlm_db = other_obc_dbs[obc_name]

        body_c = ""
        body_h = ""
        tlmdef_body_h = ""

        for tlm in tlm_db:
            tlm_name = tlm["tlm_name"]
            tlm_name_lower = tlm_name.lower()
            body_c += (
                "static DS_ERR_CODE {_obc_name_upper}_analyze_tlm_"
                + tlm_name_lower
                + "_(const CommonTlmPacket* packet, {_obc_name_upper}_TLM_CODE tlm_id, "
                + driver_type
                + "* "
                + driver_name
                + ");\n"
            )

        body_c += "\n"
        body_c += "static CommonTlmPacket {_obc_name_upper}_ctp_;\n"
        body_c += "\n"

        body_h += "typedef struct " + driver_type + " " + driver_type + ";\n"
        body_h += "\n"
        body_h += "#define {_obc_name_upper}_MAX_TLM_NUM (" + str(max_tlm_num) + ")\n"
        body_h += "\n"
        body_h += "typedef struct\n"
        body_h += "{{\n"
        body_h += "  CommonTlmPacket packet;   //!< 最新のテレメパケットを保持\n"
        body_h += "  uint8_t is_null_packet;   //!< 一度でもテレメを受信しているか？（空配列が読み出されるのを防ぐため）\n"
        body_h += "}} {_obc_name_upper}_TlmBufferElem;\n"
        body_h += "\n"
        body_h += "typedef struct\n"
        body_h += "{{\n"
        body_h += "  {_obc_name_upper}_TlmBufferElem tlm[{_obc_name_upper}_MAX_TLM_NUM];   //!< TLM ID ごとに保持\n"
        body_h += "}} {_obc_name_upper}_TlmBuffer;\n"
        body_h += "\n"

        tlmdef_body_h += "typedef struct\n"
        tlmdef_body_h += "{{\n"
        for tlm in tlm_db:
            tlm_name = tlm["tlm_name"]
            tlm_name_lower = tlm_name.lower()

            # pprint.pprint(tlm['data'][DATA_START_ROW:])
            last_var_type = ""
            tlm_struct_tree = {}  # python3.7以上を想定しているので，キーの順番は保存されていることが前提
            # tlm_struct_tree = collections.OrderedDict()     # やっぱこっちで
            for j in range(DATA_START_ROW, len(tlm["data"])):
                comment = tlm["data"][j][0]
                name = EscapeTlmElemName_(tlm["data"][j][1])
                var_type = tlm["data"][j][2]
                if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
                    break
                if comment != "":
                    continue
                if name == "":
                    continue
                if var_type == "":
                    var_type = last_var_type
                last_var_type = var_type
                if last_var_type == "":
                    continue

                # name_tree = name.lower().split(".")[2:]     # OBC名.テレメ名.HOGE.FUGA を想定
                name_tree = name.lower().split(".")
                name_path = "/".join(name_tree)
                if SetStructTree_(tlm_struct_tree, name_path, var_type):
                    print("Error: Tlm DB Struct Parse Err at " + name, file=sys.stderr)
                    sys.exit(1)

            # pprint.pprint(tlm_struct_tree)
            # for k, v in tlm_struct_tree.items():
            #     print(k)
            #     print(v)
            #     print("")

            tlmdef_body_h += GenerateStructDef_(tlm_struct_tree, tlm_name_lower)

        tlmdef_body_h += "}} {_obc_name_upper}_TlmData;\n"

        body_h += (
            "void {_obc_name_upper}_init_tlm_buffer(" + driver_type + "* " + driver_name + ");\n"
        )
        body_h += "\n"
        body_h += (
            "DS_ERR_CODE {_obc_name_upper}_buffer_tlm_packet(DS_StreamConfig* p_stream_config, "
            + driver_type
            + "* "
            + driver_name
            + ");\n"
        )
        body_h += "\n"
        body_h += (
            "TF_TLM_FUNC_ACK {_obc_name_upper}_pick_up_tlm_buffer(const "
            + driver_type
            + "* "
            + driver_name
            + ", {_obc_name_upper}_TLM_CODE tlm_id, uint8_t* packet, uint16_t* len, uint16_t max_len);\n"
        )

        body_c += (
            "void {_obc_name_upper}_init_tlm_buffer(" + driver_type + "* " + driver_name + ")\n"
        )
        body_c += "{{\n"
        body_c += "  // packet などは，上位の driver の初期化で driver もろとも memset 0x00 されていると期待して，ここではしない\n"
        body_c += "  int i = 0;\n"
        body_c += "  for (i = 0; i < {_obc_name_upper}_MAX_TLM_NUM; ++i)\n"
        body_c += "  {{\n"
        body_c += "    " + driver_name + "->tlm_buffer.tlm[i].is_null_packet = 1;\n"
        body_c += "  }}\n"
        body_c += "}}\n"
        body_c += "\n"
        body_c += (
            "DS_ERR_CODE {_obc_name_upper}_buffer_tlm_packet(DS_StreamConfig* p_stream_config, "
            + driver_type
            + "* "
            + driver_name
            + ")\n"
        )
        body_c += "{{\n"

        body_c += "  {_obc_name_upper}_TLM_CODE tlm_id;\n"
        body_c += "  DS_ERR_CODE ret;\n"
        body_c += "\n"
        body_c += "  ret = CTP_get_ctp_from_dssc(p_stream_config, &{_obc_name_upper}_ctp_);\n"
        body_c += "  if (ret != DS_ERR_CODE_OK) return ret;\n"
        body_c += "\n"
        body_c += "  tlm_id  = ({_obc_name_upper}_TLM_CODE)CTP_get_id(&{_obc_name_upper}_ctp_);\n"
        body_c += "\n"

        body_c += "  switch (tlm_id)\n"
        body_c += "  {{\n"
        for tlm in tlm_db:
            tlm_name = tlm["tlm_name"]
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()
            body_c += "  case {_obc_name_upper}_Tlm_CODE_" + tlm_name_upper + ":\n"
            body_c += (
                "    return {_obc_name_upper}_analyze_tlm_"
                + tlm_name_lower
                + "_(&{_obc_name_upper}_ctp_, tlm_id, "
                + driver_name
                + ");\n"
            )
        body_c += "  default:\n"
        body_c += "    " + settings["other_obc_data"][i]["code_when_tlm_not_found"] + "\n"
        body_c += "    return DS_ERR_CODE_ERR;\n"
        body_c += "  }}\n"
        body_c += "}}\n"
        body_c += "\n"
        for tlm in tlm_db:
            conv_tpye_to_temp = {
                "int8_t": "temp_i8",
                "int16_t": "temp_i16",
                "int32_t": "temp_i32",
                "uint8_t": "temp_u8",
                "uint16_t": "temp_u16",
                "uint32_t": "temp_u32",
                "float": "temp_f",
                "double": "temp_d",
            }
            conv_tpye_to_size = {
                "int8_t": 1,
                "int16_t": 2,
                "int32_t": 4,
                "uint8_t": 1,
                "uint16_t": 2,
                "uint32_t": 4,
                "float": 4,
                "double": 8,
            }
            tlm_name = tlm["tlm_name"]
            tlm_name_upper = tlm_name.upper()
            tlm_name_lower = tlm_name.lower()

            body_c += (
                "static DS_ERR_CODE {_obc_name_upper}_analyze_tlm_"
                + tlm_name_lower
                + "_(const CommonTlmPacket* packet, {_obc_name_upper}_TLM_CODE tlm_id, "
                + driver_type
                + "* "
                + driver_name
                + ")\n"
            )
            body_c += "{{\n"
            body_c += "  const uint8_t* f = packet->packet;\n"
            for k, v in conv_tpye_to_temp.items():
                if k == "float":
                    body_c += "  " + k + " " + v + " = 0.0f;\n"
                elif k == "double":
                    body_c += "  " + k + " " + v + " = 0.0;\n"
                else:
                    body_c += "  " + k + " " + v + " = 0;\n"
            body_c += "\n"
            body_c += "  // GS へのテレメ中継のためのバッファーへのコピー\n"
            body_c += (
                "  CTP_copy_packet(&("
                + driver_name
                + "->tlm_buffer.tlm[tlm_id].packet), packet);\n"
            )
            body_c += "  " + driver_name + "->tlm_buffer.tlm[tlm_id].is_null_packet = 0;\n"
            body_c += "  // TODO: CRC チェック\n"
            body_c += "\n"

            body_c += "  // MOBC 内部でテレメデータへアクセスしやすいようにするための構造体へのパース\n"
            last_var_type = ""
            for j in range(DATA_START_ROW, len(tlm["data"])):
                comment = tlm["data"][j][0]
                name = EscapeTlmElemName_(tlm["data"][j][1])
                var_type = tlm["data"][j][2]
                if comment == "" and name == "":  # CommentもNameも空白なら打ち切り
                    break
                if comment != "":
                    continue
                if name == "":
                    continue
                if var_type == "":
                    var_type = last_var_type
                last_var_type = var_type
                if last_var_type == "":
                    continue

                oct_pos = int(tlm["data"][j][5])
                bit_pos = int(tlm["data"][j][6])
                bit_len = int(tlm["data"][j][7])
                is_compression = 0  # テレメ圧縮フラグ for ビットフィールドをつかってる奴ら
                if tlm["data"][j][2] == "" or tlm["data"][j + 1][2] == "":
                    is_compression = 1
                if (
                    tlm["data"][j + 1][0] == ""
                    and tlm["data"][j + 1][1] == ""
                    and tlm["data"][j][2] != ""
                ):  # 最終行の除外
                    is_compression = 0

                # name_tree = name.lower().split(".")[2:]     # OBC名.テレメ名.HOGE.FUGA を想定
                name_tree = name.lower().split(".")
                name_path = ".".join(name_tree)
                var_name = driver_name + "->tlm_data." + tlm_name_lower + "." + name_path
                if is_compression:
                    body_c += (
                        "  ENDIAN_memcpy(&"
                        + conv_tpye_to_temp[var_type]
                        + ", &(f["
                        + str(oct_pos)
                        + "]), "
                        + str(conv_tpye_to_size[var_type])
                        + ");\n"
                    )
                    body_c += (
                        "  "
                        + conv_tpye_to_temp[var_type]
                        + " >>= "
                        + str(conv_tpye_to_size[var_type] * 8 - bit_pos - bit_len)
                        + ";\n"
                    )
                    body_c += (
                        "  "
                        + conv_tpye_to_temp[var_type]
                        + " &= "
                        + hex(int("0b" + "1" * bit_len, 2))
                        + ";\n"
                    )
                    body_c += "  " + var_name + " = " + conv_tpye_to_temp[var_type] + ";\n"
                else:
                    body_c += (
                        "  ENDIAN_memcpy(&("
                        + var_name
                        + "), &(f["
                        + str(oct_pos)
                        + "]), "
                        + str(conv_tpye_to_size[var_type])
                        + ");\n"
                    )

            body_c += "  // TODO: ビットフィールドをつかっている系は，様々なパターンがあり得るので，今後，バグが出ないか注視する\n"
            body_c += "\n"
            body_c += "  // ワーニング回避\n"
            for k, v in conv_tpye_to_temp.items():
                body_c += "  (void)" + v + ";\n"
            body_c += "\n"
            body_c += "  return DS_ERR_CODE_OK;\n"
            body_c += "}}\n"
            body_c += "\n"

        body_c += (
            "TF_TLM_FUNC_ACK {_obc_name_upper}_pick_up_tlm_buffer(const "
            + driver_type
            + "* "
            + driver_name
            + ", {_obc_name_upper}_TLM_CODE tlm_id, uint8_t* packet, uint16_t* len, uint16_t max_len)\n"
        )
        body_c += "{{\n"
        body_c += "  const CommonTlmPacket* buffered_packet;\n"
        body_c += "\n"
        body_c += (
            "  if (tlm_id >= {_obc_name_upper}_MAX_TLM_NUM) return TF_TLM_FUNC_ACK_NOT_DEFINED;\n"
        )
        body_c += (
            "  if ("
            + driver_name
            + "->tlm_buffer.tlm[tlm_id].is_null_packet) return TF_TLM_FUNC_ACK_NULL_PACKET;\n"
        )
        body_c += "\n"
        body_c += "  buffered_packet = &(" + driver_name + "->tlm_buffer.tlm[tlm_id].packet);\n"
        body_c += "  *len = CTP_get_packet_len(buffered_packet);\n"
        body_c += "\n"
        body_c += "  if (*len > max_len) return TF_TLM_FUNC_ACK_TOO_SHORT_LEN;\n"
        body_c += "\n"
        body_c += "  memcpy(packet, &buffered_packet->packet, (size_t)(*len));\n"
        body_c += "  return TF_TLM_FUNC_ACK_SUCCESS;\n"
        body_c += "}}\n"
        body_c += "\n"

        output_file_path = (
            settings["c2a_root_dir"]
            + r"src_user/Drivers/"
            + settings["other_obc_data"][i]["driver_path"]
        )
        OutputTlmBufferC_(
            output_file_path + obc_name.lower() + "_telemetry_buffer.c", obc_name, body_c, settings
        )
        OutputTlmBufferH_(
            output_file_path + obc_name.lower() + "_telemetry_buffer.h", obc_name, body_h, settings
        )
        OutputTlmDataDefH_(
            output_file_path + obc_name.lower() + "_telemetry_data_definitions.h",
            obc_name,
            tlmdef_body_h,
            settings,
        )


def OutputTlmBufferC_(file_path, name, body, settings):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += """
#pragma section REPRO
/**
 * @file
 * @brief  テレメトリバッファー（テレメ中継）
 * @note   このコードは自動生成されています！
 */
#include <src_core/Drivers/Protocol/common_tlm_cmd_packet_for_driver_super.h>
#include "./{_obc_name_lower}_telemetry_definitions.h"
#include "./{_obc_name_lower}_telemetry_buffer.h"
#include "./{_obc_name_lower}.h"
#include <string.h>

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
        fh.write(
            output.format(
                _obc_name_upper=name_upper, _obc_name_lower=name_lower, _obc_name_capit=name_capit
            )
        )


def OutputTlmBufferH_(file_path, name, body, settings):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += """
/**
 * @file
 * @brief  テレメトリバッファー（テレメ中継）
 * @note   このコードは自動生成されています！
 */
#ifndef {_obc_name_upper}_TELEMETRY_BUFFER_H_
#define {_obc_name_upper}_TELEMETRY_BUFFER_H_

#include "./{_obc_name_lower}_telemetry_definitions.h"
#include <src_core/Drivers/Super/driver_super.h>
#include <src_core/TlmCmd/common_tlm_packet.h>
#include <src_core/TlmCmd/telemetry_frame.h>

"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

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


def OutputTlmDataDefH_(file_path, name, body, settings):
    name_upper = name.upper()
    name_lower = name.lower()
    name_capit = name.capitalize()

    output = ""
    output += """
/**
 * @file
 * @brief  バッファリングされているテレメをパースしてMOBC内でかんたんに利用できるようにするためのテレメデータ構造体定義
 * @note   このコードは自動生成されています！
 */
#ifndef {_obc_name_upper}_TELEMETRY_DATA_DEFINITIONS_H_
#define {_obc_name_upper}_TELEMETRY_DATA_DEFINITIONS_H_

"""[
        1:
    ]  # 最初の改行を除く

    output += body

    output += """

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


def GetStructTree_(dict, path, sep="/", default=None):
    path_list = path.split(sep)

    def _(dict, path_list, sep, default):
        if len(path_list) == 0:
            return default
        if len(path_list) == 1:
            return dict.get(path_list[0], default)
        else:
            return _(dict.get(path_list[0], {}), path_list[1:], sep, default)

    return _(dict, path_list, sep=sep, default=None)


def SetStructTree_(dict, path, val, sep="/"):
    path_list = path.split(sep)

    def _(dict, path_list, val, sep):
        if len(path_list) == 0:
            return 1  # err
        if len(path_list) == 1:
            key = path_list[0]
            if key in dict:
                return 1  # 上書きエラー
            else:
                dict[key] = val
                return 0
        else:
            key = path_list[0]
            if key not in dict:
                dict[key] = {}
            return _(dict[key], path_list[1:], val, sep)

    return _(dict, path_list, val, sep=sep)


def GenerateStructDef_(tree, name):
    def _(tree, name, indent):
        output = ""
        output += " " * (indent) + "struct\n"
        output += " " * (indent) + "{{\n"
        for k, v in tree.items():
            if type(v) == dict:
                output += _(v, k, indent + 2)
                continue
            output += " " * (indent + 2) + v + " " + k + ";\n"
        output += " " * (indent) + "}} " + name + ";\n"
        return output

    return _(tree, name, 2)


def EscapeTlmElemName_(name):
    return name.replace("/", "_")
