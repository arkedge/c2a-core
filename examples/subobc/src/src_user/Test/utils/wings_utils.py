#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import json
from c2a_pytest_gaia import wings_compat


# WINGS API 互換の c2a-pytest-gaia を使用
def get_wings_operation():
    # examples/mobc と examples/subobc 双方の TlmCmdDB のデータが必要
    # そのため，examples/mobc 側で生成・merge した tlmcmddb.json を使う（詳細は examples/mobc/package.json を参照）
    tlmcmddb = json.load(open("../../../../mobc/tlmcmddb.json"))
    return wings_compat.Operation(tlmcmddb)
