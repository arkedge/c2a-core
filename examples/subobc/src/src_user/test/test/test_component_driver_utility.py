#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# 前提
# - AOBC -> MOBC に 1 Hz で AOBC_HK が流れていること

import os
import sys
import time
import binascii
import pprint

import isslwings as wings
import pytest

ROOT_PATH = "../../"
sys.path.append(os.path.dirname(__file__) + "/" + ROOT_PATH + "utils")
import c2a_enum_utils
import wings_utils

c2a_enum = c2a_enum_utils.get_c2a_enum()
mobc_c2a_enum = c2a_enum_utils.get_mobc_c2a_enum()
ope = wings_utils.get_wings_operation()

# コードと整合をとる
SUB_OBC = "AOBC"
Tlm_CODE_SUB_OBC_HK = c2a_enum.Tlm_CODE_AOBC_HK
TLM_APID_SUB_OBC = c2a_enum.APID_TLM_AOBC
CDRV_ID_AOBC = 0

# NOP だと id が 0x00 なのでちがうのを
USE_DUMMY_CMD_ID = c2a_enum.Cmd_CODE_EH_CLEAR_LOG


@pytest.mark.real
@pytest.mark.sils
def test_cdrv_util_hal_tx():
    # まず， sub OBC Cmd で GSC counter が上がることを確認
    sub_obc_gsc_cnt = get_sub_obc_hk_tlm()[SUB_OBC + "_HK.OBC.GS_CMD.COUNTER"]
    ope.send_rt_cmd(USE_DUMMY_CMD_ID, (), SUB_OBC, False)
    tlm_SUB_HK = get_sub_obc_hk_tlm()
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.LAST_EXEC.ID"] == USE_DUMMY_CMD_ID
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.LAST_EXEC.EXEC_STS"] == "SUC"
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.COUNTER"] == sub_obc_gsc_cnt + 1

    # Cmd_CDRV_UTIL_HAL_TX で送信
    sub_obc_gsc_cnt = tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.COUNTER"]
    raw_param = get_subobc_nop_cmd_eb90_frame()
    raw_param = '0x' + ''.join(f'{byte:02x}' for byte in raw_param)
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, mobc_c2a_enum.Cmd_CODE_CDRV_UTIL_HAL_TX, (CDRV_ID_AOBC, raw_param), mobc_c2a_enum.Tlm_CODE_HK
    )
    tlm_SUB_HK = get_sub_obc_hk_tlm()
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.LAST_EXEC.ID"] == USE_DUMMY_CMD_ID
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.LAST_EXEC.EXEC_STS"] == "SUC"
    assert tlm_SUB_HK[SUB_OBC + "_HK.OBC.GS_CMD.COUNTER"] == sub_obc_gsc_cnt + 1


def get_sub_obc_hk_tlm():
    time.sleep(2)
    tlm_SUB_HK = wings.util.forward_and_receive_tlm(
        ope, mobc_c2a_enum.Cmd_CODE_TG_FORWARD_TLM, TLM_APID_SUB_OBC, Tlm_CODE_SUB_OBC_HK
    )
    return tlm_SUB_HK


def get_subobc_nop_cmd_eb90_frame():
    packet_len = 15

    frame = []
    frame.append(0xEB)
    frame.append(0x09)
    frame.append(packet_len // 256)
    frame.append(packet_len % 256)

    tmp = 0
    # Version No
    tmp += 0
    # Packet Type
    tmp += 1
    tmp <<= 1
    # Sec. HDR Flag
    tmp += 1
    tmp <<= 11
    # APID
    tmp += c2a_enum.APID_CMD_TO_AOBC

    frame.append(tmp // 256)
    frame.append(tmp % 256)

    tmp = 0
    # Sequence Flags
    tmp += 3
    tmp <<= 14
    # Sequence Count
    tmp += 0

    frame.append(tmp // 256)
    frame.append(tmp % 256)

    # Packet Data Len
    packet_data_len = packet_len - 6 - 1
    frame.append(packet_data_len // 256)
    frame.append(packet_data_len % 256)

    # Sec. HDR Ver
    frame.append(1)
    # Command Type
    frame.append(0)
    # Command ID
    frame.append(USE_DUMMY_CMD_ID // 256)
    frame.append(USE_DUMMY_CMD_ID % 256)
    # Dest Type = 0 & Execution Type = 0
    frame.append(0)
    # Time Indicator
    frame.append(0)
    frame.append(0)
    frame.append(0)
    frame.append(0)

    crc_body = frame[4:]
    crc = binascii.crc_hqx(bytes(crc_body), 0xFFFF)

    frame.append(crc // 256)
    frame.append(crc % 256)

    frame.append(0xC5)
    frame.append(0x79)

    pprint.pprint(frame)

    return frame


if __name__ == "__main__":
    pass
