#!/bin/bash

echo "Move tlm-cmd-db to C2A user root directory"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

echo "  <C2A user>/src/src_user/settings/tlm_cmd/data_base -> <C2A user>/tlm-cmd-db"
mv ./src/src_user/settings/tlm_cmd/data_base ./tlm-cmd-db

echo "  rename tlmcbddb path ref"
find_all | xargs -0 sed -i -e "s#src/src_user/settings/tlm_cmd/data_base#tlm-cmd-db#g"
