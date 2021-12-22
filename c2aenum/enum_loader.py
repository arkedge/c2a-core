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
        p_with_id = re.compile(r"^  ({}\w+) = (\w+)".format(prefix))
        p_without_id = re.compile(r"^  ({}\w+)".format(prefix))

        with open(path, encoding="shift_jis") as f:

            last_enum_id = 0
            for line in f.readlines():
                m_with_id = p_with_id.match(line)
                m_without_id = p_without_id.match(line)
                if not m_with_id and not m_without_id:
                    continue

                if m_with_id:
                    enum_name = m_with_id.group(1)
                    enum_id   = m_with_id.group(2)
                else:
                    enum_name = m_without_id.group(1)
                    enum_id   = str(last_enum_id + 1)

                if enum_id[:2] == "0x":
                    enum_id = int(enum_id, base=16)
                else:
                    enum_id = int(enum_id, base=10)

                self.__setattr__(enum_name, enum_id)
                last_enum_id = enum_id

    def _load_non_numbered_enum_from_file(self, path, prefix):
        # TBD
        pass


def load_enum(c2a_src_path) -> C2aEnum:
    c2a_enum = C2aEnum(c2a_src_path)
    return c2a_enum


if __name__ == "__main__":
    # 単なる動作確認用
    c2a_enum = load_enum(os.path.dirname(__file__) + "/../../..")
