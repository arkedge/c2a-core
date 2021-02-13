# coding: UTF-8
"""
DB読み込み
"""

import os
import csv
import re           # 正規表現

def LoadCmdCSV(settings):
    cmd_db_path = settings["c2a_root_dir"] + r"src_user/Settings/CmdTlm/DataBase/CMD_DB/"
    sgc_db_path = cmd_db_path + settings["db_prefix"] + "_CMD_DB_CMD_DB.csv"        # single cmd
    bct_db_path = cmd_db_path + settings["db_prefix"] + "_CMD_DB_BCT.csv"       # block cmd table

    with open(sgc_db_path, mode='r', encoding='shift_jis') as fh:
        reader = csv.reader(fh)
        sgc_db = [row for row in reader]
    with open(bct_db_path, mode='r', encoding='shift_jis') as fh:
        reader = csv.reader(fh)
        bct_db = [row for row in reader]

    # print(sgc_db)
    # print(bct_db)
    return {'sgc': sgc_db, 'bct': bct_db}


def LoadTlmCSV(settings):
    tlm_db_path = settings["c2a_root_dir"] + r"src_user/Settings/CmdTlm/DataBase/TLM_DB/calced_data/"

    tlm_names = [file for file in os.listdir(tlm_db_path) if file.endswith(".csv")]
    regex = r"^" + settings["db_prefix"] + "_TLM_DB_"
    tlm_names = [re.sub(regex, "", file) for file in tlm_names]
    tlm_names = [re.sub(".csv$", "", file) for file in tlm_names]
    # pprint.pprint(tlm_names)
    # print(len(tlm_names))

    tlm_db = []

    for tlm_name in tlm_names:
        tlm_sheet_path = tlm_db_path + settings["db_prefix"] + "_TLM_DB_" + tlm_name + ".csv"
        with open(tlm_sheet_path, mode='r', encoding='shift_jis') as fh:
            reader = csv.reader(fh)
            sheet = [row for row in reader]
            # pprint.pprint(sheet)
            # print(sheet)
            tlm_id = sheet[1][2]      # テレメIDを取得．マジックナンバーで指定してしまってる．
            tlm_db.append({'tlm_id': tlm_id, 'tlm_name': tlm_name, 'data': sheet})
            # tlm_db.append({'tlm_id': tlm_id, 'tlm_name': tlm_name, 'data': 1})

    tlm_db.sort(key=lambda x: x['tlm_id'])

    # 重複チェックをする

    return tlm_db;

