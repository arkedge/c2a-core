#!/bin/bash

mv ./src/src_user/settings/tlm_cmd/data_base ./tlm-cmd-db

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src/src_user/settings/tlm_cmd/data_base#tlm-cmd-db#g"
