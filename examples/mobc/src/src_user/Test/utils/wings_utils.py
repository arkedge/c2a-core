#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import json
from c2a_pytest_gaia import wings_compat


def get_wings_operation():
    tlmcmddb = json.load(open("../../../tlmcmddb.json"))
    return wings_compat.Operation(tlmcmddb)
