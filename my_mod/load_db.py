# coding: UTF-8
"""
DB読み込み
"""

import os
import sys
import csv
import re  # 正規表現

# import pprint


def LoadCmdDb(settings):
    cmd_db_path = settings["c2a_root_dir"] + r"src_user/Settings/TlmCmd/DataBase/CMD_DB/"

    sgc_db, bct_db = LoadCmdCSV_(
        cmd_db_path, settings["db_prefix"], settings["input_file_encoding"]
    )

    other_obc_dbs = {}
    if settings["is_main_obc"]:
        other_obc_dbs = LoadOtherObcCmd_(settings)

    # TODO: 重複チェックをする

    # print(sgc_db)
    # print(bct_db)
    return {"sgc": sgc_db, "bct": bct_db, "other_obc": other_obc_dbs}


def LoadCmdCSV_(cmd_db_path, db_prefix, encoding):
    sgc_db_path = cmd_db_path + db_prefix + "_CMD_DB_CMD_DB.csv"  # single cmd
    bct_db_path = cmd_db_path + db_prefix + "_CMD_DB_BCT.csv"  # block cmd table

    with open(sgc_db_path, mode="r", encoding=encoding) as fh:
        reader = csv.reader(fh)
        sgc_db = [row for row in reader]
    with open(bct_db_path, mode="r", encoding=encoding) as fh:
        reader = csv.reader(fh)
        bct_db = [row for row in reader]

    return sgc_db, bct_db


def LoadTlmDb(settings):
    tlm_db_path = (
        settings["c2a_root_dir"] + r"src_user/Settings/TlmCmd/DataBase/TLM_DB/calced_data/"
    )

    tlm_db = LoadTlmCSV_(
        tlm_db_path,
        settings["db_prefix"],
        settings["tlm_id_range"],
        settings["input_file_encoding"],
    )

    other_obc_dbs = {}
    if settings["is_main_obc"]:
        other_obc_dbs = LoadOtherObcTlm(settings)

    # TODO: 重複チェックをする

    return {"tlm": tlm_db, "other_obc": other_obc_dbs}


def LoadTlmCSV_(tlm_db_path, db_prefix, tlm_id_range, encoding):
    tlm_names = [file for file in os.listdir(tlm_db_path) if file.endswith(".csv")]
    regex = r"^" + db_prefix + "_TLM_DB_"
    tlm_names = [re.sub(regex, "", file) for file in tlm_names]
    tlm_names = [re.sub(".csv$", "", file) for file in tlm_names]
    # pprint.pprint(tlm_names)
    # print(len(tlm_names))

    tlm_db = []

    for tlm_name in tlm_names:
        tlm_sheet_path = tlm_db_path + db_prefix + "_TLM_DB_" + tlm_name + ".csv"
        with open(tlm_sheet_path, mode="r", encoding=encoding) as fh:
            reader = csv.reader(fh)
            sheet = [row for row in reader]
            # pprint.pprint(sheet)
            # print(sheet)
            enable_flag = sheet[2][2]  # FIXME: Enable/Disable を取得．マジックナンバーで指定してしまってる．
            if enable_flag != "ENABLE":
                continue
            tlm_id = sheet[1][2]  # FIXME: テレメIDを取得．マジックナンバーで指定してしまってる．
            if not int(tlm_id_range[0], 0) <= int(tlm_id, 0) < int(tlm_id_range[1], 0):
                print(
                    "Error: TLM ID is invalid at " + db_prefix + "_TLM_DB_" + tlm_name + ".csv",
                    file=sys.stderr,
                )
                sys.exit(1)
            raw_local_vars = (
                sheet[1][3].replace("%%", "").split("##")
            )  # FIXME: ローカル変数を取得．マジックナンバーで指定してしまってる．
            local_vars = []
            for raw_local_var in raw_local_vars:
                local_var = raw_local_var.strip().replace("@@", ",")
                if len(local_var) > 0:
                    local_vars.append(local_var)
            tlm_db.append(
                {"tlm_id": tlm_id, "tlm_name": tlm_name, "local_vars": local_vars, "data": sheet}
            )
            # tlm_db.append({'tlm_id': tlm_id, 'tlm_name': tlm_name, 'data': 1})

    tlm_db.sort(key=lambda x: x["tlm_id"])

    return tlm_db


def LoadOtherObcCmd_(settings):
    other_obc_dbs = {}

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        cmd_db_path = settings["other_obc_data"][i]["db_path"] + r"CMD_DB/"
        sgc_db, bct_db = LoadCmdCSV_(
            cmd_db_path,
            settings["other_obc_data"][i]["db_prefix"],
            settings["other_obc_data"][i]["input_file_encoding"],
        )
        # other_obc_dbs.append(sgc_db)
        other_obc_dbs[settings["other_obc_data"][i]["name"]] = sgc_db
        # print(i)

    # pprint.pprint(settings["other_obc_data"])
    # pprint.pprint(other_obc_dbs)
    return other_obc_dbs


def LoadOtherObcTlm(settings):
    other_obc_dbs = {}

    for i in range(len(settings["other_obc_data"])):
        if not settings["other_obc_data"][i]["is_enable"]:
            continue
        tlm_db_path = settings["other_obc_data"][i]["db_path"] + r"TLM_DB/calced_data/"

        tlm_db = LoadTlmCSV_(
            tlm_db_path,
            settings["other_obc_data"][i]["db_prefix"],
            settings["other_obc_data"][i]["tlm_id_range"],
            settings["other_obc_data"][i]["input_file_encoding"],
        )
        other_obc_dbs[settings["other_obc_data"][i]["name"]] = tlm_db

    # pprint.pprint(other_obc_dbs)
    return other_obc_dbs
