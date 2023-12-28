#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

import isslwings as wings
import pytest

ROOT_PATH = "../../../"
sys.path.append(os.path.dirname(__file__) + "/" + ROOT_PATH + "utils")
import c2a_enum_utils
import wings_utils

c2a_enum = c2a_enum_utils.get_c2a_enum()
ope = wings_utils.get_wings_operation()

# コードと整合をとる
CDIS_MGR_MAX_NUM_OF_CDIS = 8
BCT_MAX_BLOCKS = 383
CDIS_IDX_OF_TL_GS = 3


class cmd:
    ti: int
    id: int
    params: list


@pytest.mark.real
@pytest.mark.sils
def test_ccp_dump_cmd_assertion():
    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_MGR_MAX_NUM_OF_CDIS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "NOT_FOUND"

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_BCT, (BCT_MAX_BLOCKS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "BCT"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "NOT_FOUND"

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_MGR_MAX_NUM_OF_CDIS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "NOT_FOUND"


@pytest.mark.real
@pytest.mark.sils
def test_ccp_dump_cdis():
    # 空の TL0
    clear_tl0()
    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_IDX_OF_TL_GS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "NOT_FOUND"

    # TL 登録
    now_ti = tlm_CCP_DUMP["CCP_DUMP.SH.TI"]

    tlc0 = init_cmd_class(
        now_ti + 10000, c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM, [0x12, 0x34, 0x56]
    )
    tlc1 = init_cmd_class(
        now_ti + 20000, c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM, [0xAB, 0xCD, 0xEF]
    )

    wings.util.send_tl_cmd(ope, tlc0.ti, tlc0.id, (0x1234, 0x56))
    wings.util.send_tl_cmd(ope, tlc0.ti, tlc0.id, (0xABCD, 0xEF))

    # TL 確認
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_IDX_OF_TL_GS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "OK"
    assert tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] > 0
    assert (
        tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] < 100
    )
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.VER"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.SH_FLAG"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.PACKET_LEN"] == 11
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.VER"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_ID"] == tlc0.id
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.DEST_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.EXEC_TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.TI"] == tlc0.ti
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM0"] == tlc0.params[0]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM1"] == tlc0.params[1]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM2"] == tlc0.params[2]

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_IDX_OF_TL_GS, 1), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "OK"
    assert tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] > 0
    assert (
        tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] < 100
    )
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.VER"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.SH_FLAG"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.PACKET_LEN"] == 11
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.VER"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_ID"] == tlc1.id
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.DEST_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.EXEC_TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.TI"] == tlc1.ti
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM0"] == tlc1.params[0]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM1"] == tlc1.params[1]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM2"] == tlc1.params[2]

    # 空の TL0
    clear_tl0()
    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_IDX_OF_TL_GS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "NOT_FOUND"


@pytest.mark.real
@pytest.mark.sils
def test_ccp_dump_bct():
    # BCT は空では不定がはいってるだけ． Cmd_CCP_DUMP_BCT で空を取得してもエラーにならない
    clear_bct(c2a_enum.BC_TEST_USE_PYTEST)

    # BCT 登録

    bc0 = init_cmd_class(10000, c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM, [0x12, 0x34, 0x56])
    bc1 = init_cmd_class(20000, c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM, [0xAB, 0xCD, 0xEF])

    wings.util.send_bl_cmd(ope, bc0.ti, bc0.id, (0x1234, 0x56))
    wings.util.send_bl_cmd(ope, bc0.ti, bc0.id, (0xABCD, 0xEF))

    # BCT 確認
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_BCT, (c2a_enum.BC_TEST_USE_PYTEST, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "BCT"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "OK"
    assert tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] > 0
    assert (
        tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] < 100
    )
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.VER"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.SH_FLAG"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.PACKET_LEN"] == 11
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.VER"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_ID"] == bc0.id
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.DEST_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.EXEC_TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.TI"] == bc0.ti
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM0"] == bc0.params[0]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM1"] == bc0.params[1]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM2"] == bc0.params[2]

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_BCT, (c2a_enum.BC_TEST_USE_PYTEST, 1), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.TARGET"] == "BCT"
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.STATUS"] == "OK"
    assert tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] > 0
    assert (
        tlm_CCP_DUMP["CCP_DUMP.SH.TI"] - tlm_CCP_DUMP["CCP_DUMP.DUMP.DUMP_TIME.TOTAL_CYCLE"] < 100
    )
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.VER"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.SH_FLAG"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PH.PACKET_LEN"] == 11
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.VER"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.CMD_ID"] == bc1.id
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.DEST_TYPE"] == 0
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.EXEC_TYPE"] == 1
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.SH.TI"] == bc1.ti
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM0"] == bc1.params[0]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM1"] == bc1.params[1]
    assert tlm_CCP_DUMP["CCP_DUMP.DUMP.CCP.PATAM2"] == bc1.params[2]


def get_ccp_dump_tlm():
    tlm = wings.util.generate_and_receive_tlm(
        ope, c2a_enum.Cmd_CODE_TG_GENERATE_RT_TLM, c2a_enum.Tlm_CODE_CCP_DUMP
    )
    return tlm


def clear_tl0():
    # TL0 をクリア
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TLCD_CLEAR_ALL_TIMELINE,
        (c2a_enum.TLCD_ID_FROM_GS,),
        c2a_enum.Tlm_CODE_HK,
    )


def clear_bct(id):
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_BCT_CLEAR_BLOCK, (id,), c2a_enum.Tlm_CODE_HK
    )


def init_cmd_class(ti, id, params):
    cmd.ti = ti
    cmd.id = id
    cmd.params = params
    return cmd


if __name__ == "__main__":
    pass
