#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import time

import isslwings as wings
import pytest

ROOT_PATH = "../../../"
sys.path.append(os.path.dirname(__file__) + "/" + ROOT_PATH + "utils")
import c2a_enum_utils
import wings_utils

c2a_enum = c2a_enum_utils.get_c2a_enum()
ope = wings_utils.get_wings_operation()

# コードと整合をとる
CDIS_IDX_OF_TL_GS = 3
APP_ID_OF_TL_GS = c2a_enum.AR_TLC_DISPATCHER_GS
SEC_TO_TI = 10


@pytest.mark.sils
@pytest.mark.real
def test_cdis_mgr_cmd():
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM, (0,), c2a_enum.Tlm_CODE_HK
    )
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS_MGR.IDX_FOR_TLM"] == 0
    cdis_adr = tlm_CDIS["CDIS.CDIS.ADR"]

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM, (CDIS_IDX_OF_TL_GS,), c2a_enum.Tlm_CODE_HK
    )
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS_MGR.IDX_FOR_TLM"] == CDIS_IDX_OF_TL_GS
    assert tlm_CDIS["CDIS.CDIS.ADR"] != cdis_adr

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM,
        (tlm_CDIS["CDIS.CDIS_MGR.NUM_OF_CDIS"] + 1,),
        c2a_enum.Tlm_CODE_HK,
    )


@pytest.mark.sils
@pytest.mark.real
def test_cdis_mgr_idx():
    # TL0 (TL_GS) をつかって，初期化しても idx が変わらないことを実証する

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM, (CDIS_IDX_OF_TL_GS,), c2a_enum.Tlm_CODE_HK
    )
    tlm_CDIS = get_cdis_tlm()
    idx = tlm_CDIS["CDIS.CDIS.IDX"]
    cdis_adr = tlm_CDIS["CDIS.CDIS.ADR"]

    # TL0 を初期化
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_AM_INITIALIZE_APP,
        (c2a_enum.AR_TLC_DISPATCHER_GS,),
        c2a_enum.Tlm_CODE_HK,
    )

    # idx などが変わってないことを確認
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS.IDX"] == idx
    assert tlm_CDIS["CDIS.CDIS.ADR"] == cdis_adr

    # CDIS が初期化されていることを確認
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.TOTAL_CYCLE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.MODE_CYCLE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.STEP"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.CODE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PL.EXECUTED_NODES"] == 0
    assert tlm_CDIS["CDIS.CDIS.PL.ACTIVE_NODES"] == 0
    assert tlm_CDIS["CDIS.PL.COUNTER"] == 0
    assert tlm_CDIS["CDIS.PL.QUEUED"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_TIME"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_ID"] == 0

    # TL0 で Gmd 実行
    time_offset_sec = 10
    ti_offset = time_offset_sec * SEC_TO_TI
    target_ti = tlm_CDIS["CDIS.SH.TI"] + ti_offset
    wings.util.send_tl_cmd(
        ope, target_ti, c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM, (CDIS_IDX_OF_TL_GS,)
    )

    # TL0 の登録を確認
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS.PL.EXECUTED_NODES"] == 0
    assert tlm_CDIS["CDIS.CDIS.PL.ACTIVE_NODES"] == 1
    assert tlm_CDIS["CDIS.PL.COUNTER"] == 0
    assert tlm_CDIS["CDIS.PL.QUEUED"] == 1
    assert tlm_CDIS["CDIS.PL.NEXT_TIME"] == target_ti
    assert tlm_CDIS["CDIS.PL.NEXT_ID"] == c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM

    # 実行を確認
    time.sleep(time_offset_sec)
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.TOTAL_CYCLE"] == target_ti
    assert tlm_CDIS["CDIS.CDIS.PREV.CODE"] == c2a_enum.Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM
    assert tlm_CDIS["CDIS.CDIS.PL.EXECUTED_NODES"] == 1
    assert tlm_CDIS["CDIS.CDIS.PL.ACTIVE_NODES"] == 0
    assert tlm_CDIS["CDIS.PL.COUNTER"] == 1
    assert tlm_CDIS["CDIS.PL.QUEUED"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_TIME"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_ID"] == 0

    # 初期化確認
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_AM_INITIALIZE_APP,
        (c2a_enum.AR_TLC_DISPATCHER_GS,),
        c2a_enum.Tlm_CODE_HK,
    )

    # idx などが変わってないことを確認
    tlm_CDIS = get_cdis_tlm()
    assert tlm_CDIS["CDIS.CDIS.IDX"] == idx
    assert tlm_CDIS["CDIS.CDIS.ADR"] == cdis_adr

    # CDIS が初期化されていることを確認
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.TOTAL_CYCLE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.MODE_CYCLE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.TIME.STEP"] == 0
    assert tlm_CDIS["CDIS.CDIS.PREV.CODE"] == 0
    assert tlm_CDIS["CDIS.CDIS.PL.EXECUTED_NODES"] == 0
    assert tlm_CDIS["CDIS.CDIS.PL.ACTIVE_NODES"] == 0
    assert tlm_CDIS["CDIS.PL.COUNTER"] == 0
    assert tlm_CDIS["CDIS.PL.QUEUED"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_TIME"] == 0
    assert tlm_CDIS["CDIS.PL.NEXT_ID"] == 0


def get_cdis_tlm():
    return wings.util.generate_and_receive_tlm(
        ope, c2a_enum.Cmd_CODE_TG_GENERATE_RT_TLM, c2a_enum.Tlm_CODE_CDIS
    )


if __name__ == "__main__":
    test_cdis_mgr_cmd()
    pass
