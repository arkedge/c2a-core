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
BCT_MAX_BLOCKS = 383
TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE = 64
BCT_MAX_CMD_NUM = 32


class Cmd:
    ti: int
    id: int
    exec_type: int
    params: list
    digest: int


@pytest.mark.real
@pytest.mark.sils
def test_tl_bct_digest_cmd_assertion():
    # App 初期化
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_AM_INITIALIZE_APP,
        (c2a_enum.AR_TL_BCT_DIGEST,),
        c2a_enum.Tlm_CODE_HK,
    )

    # TL, BCT 初期化
    clear_tl0()
    clear_bct(c2a_enum.BC_TEST_USE_PYTEST)

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL, (c2a_enum.TLCD_ID_MAX, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_MAX
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 0
    assert tlm["TL_DIGEST.INFO.STATUS"] == "PARAM_ERR"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["TL_DIGEST.INFO.QUEUED"] == 0
    assert tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] == 0
    assert tlm["TL_DIGEST.INFO.TIME_STAMP.STEP"] == 0
    check_tl_digest(tlm, [])

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL,
        (c2a_enum.TLCD_ID_FROM_GS, 255),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_FROM_GS
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 255
    assert tlm["TL_DIGEST.INFO.STATUS"] == "NO_CCP"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["TL_DIGEST.INFO.QUEUED"] == 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_tl_digest(tlm, [])

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_TL_BCT_DIGEST_BCT, (c2a_enum.BCT_MAX_BLOCKS,), c2a_enum.Tlm_CODE_HK
    )
    tlm = get_bct_digest_tlm()
    assert tlm["BCT_DIGEST.INFO.BLOCK"] == c2a_enum.BCT_MAX_BLOCKS
    assert tlm["BCT_DIGEST.INFO.STATUS"] == "PARAM_ERR"
    assert tlm["BCT_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] == 0
    assert tlm["BCT_DIGEST.INFO.TIME_STAMP.STEP"] == 0
    check_tl_digest(tlm, [])


def check_tl_digest(tlm, digests):
    for i in range(TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE):
        if i < len(digests):
            assert digests[i] == int(tlm["TL_DIGEST.DIGESTS" + str(i)], 16)
        else:
            assert 0 == int(tlm["TL_DIGEST.DIGESTS" + str(i)], 16)


def get_tl_digest_tlm():
    tlm = wings.util.generate_and_receive_tlm(
        ope, c2a_enum.Cmd_CODE_TG_GENERATE_RT_TLM, c2a_enum.Tlm_CODE_TL_DIGEST
    )
    return tlm


def get_bct_digest_tlm():
    tlm = wings.util.generate_and_receive_tlm(
        ope, c2a_enum.Cmd_CODE_TG_GENERATE_RT_TLM, c2a_enum.Tlm_CODE_BCT_DIGEST
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


def init_cmd_class(ti, id, exec_type, params):
    cmd = Cmd()
    cmd.ti = ti
    cmd.id = id
    cmd.exec_type = exec_type
    cmd.params = params
    return cmd


if __name__ == "__main__":
    pass
