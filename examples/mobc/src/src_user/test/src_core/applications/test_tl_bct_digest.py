#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import binascii
import pprint

import isslwings as wings
import time
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
    params: list  # uint の引数のみ対応．今回のテストの趣旨としてこれでOKとした．
    param_size: list
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
        ope, c2a_enum.Cmd_CODE_TL_BCT_DIGEST_BCT, (BCT_MAX_BLOCKS,), c2a_enum.Tlm_CODE_HK
    )
    tlm = get_bct_digest_tlm()
    assert tlm["BCT_DIGEST.INFO.BLOCK"] == BCT_MAX_BLOCKS
    assert tlm["BCT_DIGEST.INFO.STATUS"] == "PARAM_ERR"
    assert tlm["BCT_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] == 0
    assert tlm["BCT_DIGEST.INFO.TIME_STAMP.STEP"] == 0
    check_bct_digest(tlm, [])


@pytest.mark.real
@pytest.mark.sils
def test_tl_digest():
    # === 空 ===
    clear_tl0()
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL,
        (c2a_enum.TLCD_ID_FROM_GS, 0),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_FROM_GS
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 0
    assert tlm["TL_DIGEST.INFO.STATUS"] == "NO_CCP"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["TL_DIGEST.INFO.QUEUED"] == 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_tl_digest(tlm, [])

    # === 2 つ登録 ===
    digest_num = 2
    digest = []
    offset_ti = tlm["TL_DIGEST.SH.TI"] + 10000
    cmd = init_cmd_class(
        offset_ti,
        c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM,
        c2a_enum.CCP_EXEC_TYPE_TL_FROM_GS,
        [0x1234, 0x56],
        [2, 1],
    )
    digest.append(cmd.digest)
    register_cmd(cmd)

    cmd = init_cmd_class(
        offset_ti + 1, c2a_enum.Cmd_CODE_NOP, c2a_enum.CCP_EXEC_TYPE_TL_FROM_GS, [], []
    )
    digest.append(cmd.digest)
    register_cmd(cmd)

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL,
        (c2a_enum.TLCD_ID_FROM_GS, 0),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_FROM_GS
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 0
    assert tlm["TL_DIGEST.INFO.STATUS"] == "OK"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == digest_num
    assert tlm["TL_DIGEST.INFO.QUEUED"] == digest_num
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_tl_digest(tlm, digest)

    # === 復数ページのためにさらに TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE だけ登録 ===
    offset_ti += 1000
    for i in range(TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE):
        cmd = init_cmd_class(
            offset_ti + i, c2a_enum.Cmd_CODE_NOP, c2a_enum.CCP_EXEC_TYPE_TL_FROM_GS, [], []
        )
        digest.append(cmd.digest)
        register_cmd(cmd)

    # 1ページ目
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL,
        (c2a_enum.TLCD_ID_FROM_GS, 0),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_FROM_GS
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 0
    assert tlm["TL_DIGEST.INFO.STATUS"] == "OK"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE
    assert tlm["TL_DIGEST.INFO.QUEUED"] == digest_num + TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_tl_digest(tlm, digest[0:TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE])

    # 2ページ目
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_TL,
        (c2a_enum.TLCD_ID_FROM_GS, 1),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_tl_digest_tlm()
    assert tlm["TL_DIGEST.INFO.TL_ID"] == c2a_enum.TLCD_ID_FROM_GS
    assert tlm["TL_DIGEST.INFO.PAGE_NO"] == 1
    assert tlm["TL_DIGEST.INFO.STATUS"] == "OK"
    assert tlm["TL_DIGEST.INFO.DIGESTS_NUM"] == digest_num
    assert tlm["TL_DIGEST.INFO.QUEUED"] == digest_num + TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["TL_DIGEST.SH.TI"] - tlm["TL_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_tl_digest(
        tlm,
        digest[TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE : TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE + digest_num],
    )


@pytest.mark.real
@pytest.mark.sils
def test_bct_digest():
    # === 空 ===
    clear_bct(c2a_enum.BC_TEST_USE_PYTEST)
    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_BCT,
        (c2a_enum.BC_TEST_USE_PYTEST,),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_bct_digest_tlm()
    assert tlm["BCT_DIGEST.INFO.BLOCK"] == c2a_enum.BC_TEST_USE_PYTEST
    assert tlm["BCT_DIGEST.INFO.STATUS"] == "OK"
    assert tlm["BCT_DIGEST.INFO.DIGESTS_NUM"] == 0
    assert tlm["BCT_DIGEST.SH.TI"] - tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["BCT_DIGEST.SH.TI"] - tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_bct_digest(tlm, [])

    # === 2 つ登録 ===
    digest_num = 2
    digest = []
    offset_ti = 0
    cmd = init_cmd_class(
        offset_ti,
        c2a_enum.Cmd_CODE_TG_FORWARD_AS_RT_TLM,
        c2a_enum.CCP_EXEC_TYPE_BC,
        [0x1234, 0x56],
        [2, 1],
    )
    digest.append(cmd.digest)
    register_cmd(cmd)

    cmd = init_cmd_class(offset_ti + 1, c2a_enum.Cmd_CODE_NOP, c2a_enum.CCP_EXEC_TYPE_BC, [], [])
    digest.append(cmd.digest)
    register_cmd(cmd)

    assert "SUC" == wings.util.send_rt_cmd_and_confirm(
        ope,
        c2a_enum.Cmd_CODE_TL_BCT_DIGEST_BCT,
        (c2a_enum.BC_TEST_USE_PYTEST,),
        c2a_enum.Tlm_CODE_HK,
    )
    tlm = get_bct_digest_tlm()
    assert tlm["BCT_DIGEST.INFO.BLOCK"] == c2a_enum.BC_TEST_USE_PYTEST
    assert tlm["BCT_DIGEST.INFO.STATUS"] == "OK"
    assert tlm["BCT_DIGEST.INFO.DIGESTS_NUM"] == digest_num
    assert tlm["BCT_DIGEST.SH.TI"] - tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] > 0
    assert tlm["BCT_DIGEST.SH.TI"] - tlm["BCT_DIGEST.INFO.TIME_STAMP.TOTAL_CYCLE"] < 100
    check_bct_digest(tlm, digest)


def check_tl_digest(tlm, digests):
    for i in range(TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE):
        if i < len(digests):
            print("check_tl_digest: " + hex(digests[i]), " : ", tlm["TL_DIGEST.DIGESTS" + str(i)])
            assert digests[i] == int(tlm["TL_DIGEST.DIGESTS" + str(i)], 16)
        else:
            assert 0 == int(tlm["TL_DIGEST.DIGESTS" + str(i)], 16)


def check_bct_digest(tlm, digests):
    for i in range(BCT_MAX_CMD_NUM):
        if i < len(digests):
            print("check_bct_digest: " + hex(digests[i]), " : ", tlm["BCT_DIGEST.DIGESTS" + str(i)])
            assert digests[i] == int(tlm["BCT_DIGEST.DIGESTS" + str(i)], 16)
        else:
            assert 0 == int(tlm["BCT_DIGEST.DIGESTS" + str(i)], 16)


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


def init_cmd_class(ti, id, exec_type, params, param_size):
    cmd = Cmd()
    cmd.ti = ti
    cmd.id = id
    cmd.exec_type = exec_type
    cmd.params = params
    cmd.param_size = param_size
    cmd.digest = calc_cmd_digest(cmd)
    return cmd


def calc_cmd_digest(cmd):
    data = []

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
    tmp += c2a_enum.APID_CMD_TO_MOBC

    data.append(tmp // 256)
    data.append(tmp % 256)

    tmp = 0
    # Sequence Flags
    tmp += 3
    tmp <<= 14
    # Sequence Count
    tmp += 0

    data.append(tmp // 256)
    data.append(tmp % 256)

    # Packet Data Len
    packet_data_len = 15 + sum(cmd.param_size) - 6 - 1
    data.append(packet_data_len // 256)
    data.append(packet_data_len % 256)

    # Sec. HDR Ver
    data.append(1)
    # Command Type
    data.append(0)
    # Command ID
    data.append(cmd.id // 256)
    data.append(cmd.id % 256)
    # Dest Type & Execution Type
    # Dest Type は 0
    data.append(cmd.exec_type)
    # Time Indicator
    data.append((cmd.ti // 256**3) % 256)
    data.append((cmd.ti // 256**2) % 256)
    data.append((cmd.ti // 256) % 256)
    data.append(cmd.ti % 256)

    for i in range(len(cmd.params)):
        if cmd.param_size[i] >= 4:
            data.append((cmd.params[i] // 256**3) % 256)
        if cmd.param_size[i] >= 3:
            data.append((cmd.params[i] // 256**2) % 256)
        if cmd.param_size[i] >= 2:
            data.append((cmd.params[i] // 256) % 256)
        if cmd.param_size[i] >= 1:
            data.append(cmd.params[i] % 256)

    pprint.pprint(data)
    return binascii.crc_hqx(bytes(data), 0xFFFF)


def register_cmd(cmd):
    if cmd.exec_type == c2a_enum.CCP_EXEC_TYPE_TL_FROM_GS:
        wings.util.send_tl_cmd(ope, cmd.ti, cmd.id, tuple(cmd.params))
        time.sleep(0.2)
    elif cmd.exec_type == c2a_enum.CCP_EXEC_TYPE_BC:
        ope.send_bl_cmd(cmd.ti, cmd.id, tuple(cmd.params))
        time.sleep(0.2)
    else:
        assert False


if __name__ == "__main__":
    pass
