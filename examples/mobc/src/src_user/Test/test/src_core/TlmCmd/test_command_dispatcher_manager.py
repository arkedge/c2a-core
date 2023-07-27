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


@pytest.mark.sils
@pytest.mark.real
def test_cdis_mgr():
    # TL0 (TL_GS) をつかって，初期化しても idx が変わらないことを実証する



if __name__ == "__main__":
    test_cdis_mgr()
    pass
