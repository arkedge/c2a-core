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
CDIS_MGR_MAX_NUM_OF_CDIS = 7
BCT_MAX_BLOCKS = 383


@pytest.mark.real
@pytest.mark.sils
def test_ccp_dump_cmd_assertion():
    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_MGR_MAX_NUM_OF_CDIS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["DUMP.STATUS"] == "NOT_FOUND"

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_BCT, (BCT_MAX_BLOCKS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["DUMP.TARGET"] == "BCT"
    assert tlm_CCP_DUMP["DUMP.STATUS"] == "NOT_FOUND"

    assert "PRM" == wings.util.send_rt_cmd_and_confirm(
        ope, c2a_enum.Cmd_CODE_CCP_DUMP_CDIS, (CDIS_MGR_MAX_NUM_OF_CDIS, 0), c2a_enum.Tlm_CODE_HK
    )
    tlm_CCP_DUMP = get_ccp_dump_tlm()
    assert tlm_CCP_DUMP["DUMP.TARGET"] == "CDIS"
    assert tlm_CCP_DUMP["DUMP.STATUS"] == "NOT_FOUND"


def get_ccp_dump_tlm():
    tlm = wings.util.generate_and_receive_tlm(
        ope, c2a_enum.Cmd_CODE_TG_GENERATE_RT_TLM, c2a_enum.Tlm_CODE_CCP_DUMP
    )
    return tlm


if __name__ == "__main__":
    pass
