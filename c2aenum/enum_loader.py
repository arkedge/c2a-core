#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re

class C2aEnum:
    def __init__(self, c2a_src_path):
        self.path = c2a_src_path

        self._load_bc()
        self._load_tlm_code()
        self._load_cmd_code()
        self._load_app()
        self._load_anomaly()
        self._load_mode()
        self._load_exec_sts()
        self._load_tl_id()

    def _load_bc(self):
        self._load_numbered_enum_from_file(
            "/src_user/CmdTlm/block_command_definitions.h", "BC_"
        )

    def _load_tlm_code(self):
        self._load_numbered_enum_from_file(
            "/src_user/CmdTlm/telemetry_definitions.h", "Tlm_CODE_"
        )

    def _load_cmd_code(self):
        self._load_numbered_enum_from_file(
            "/src_user/CmdTlm/command_definitions.h", "Cmd_CODE_"
        )

    def _load_app(self):
        pass

    def _load_anomaly(self):
        pass

    def _load_mode(self):
        pass

    def _load_exec_sts(self):
        self._load_numbered_enum_from_file(
            "/src_core/CmdTlm/common_tlm_cmd_packet.h", "CCP_EXEC_"
        )

    def _load_tl_id(self):
        self._load_numbered_enum_from_file(
            "/src_core/Applications/timeline_command_dispatcher.h", "TL_ID_"
        )

    def _load_numbered_enum_from_file(self, path, prefix):
        path = self.path + path
        p = re.compile("^  ({}\w+) = (\w+)".format(prefix))

        with open(path, encoding="shift_jis") as f:

            for line in f.readlines():
                m = p.match(line)
                if not m:
                    continue

                enum_name = m.group(1)
                enum_id   = m.group(2)

                if enum_id[:2] == "0x":
                    enum_id = int(enum_id, base=16)
                else:
                    enum_id = int(enum_id, base=10)

                self.__setattr__(enum_name, enum_id)

    def _load_non_numbered_enum_from_file(self, path, prefix):
        # TBD
        pass


def load_enum(c2a_src_path) -> C2aEnum:
    c2a_enum = C2aEnum(c2a_src_path)
    return c2a_enum


if __name__ == "__main__":
    # 単なる動作確認用
    c2a_enum = load_enum(os.path.dirname(__file__) + "/../../..")
