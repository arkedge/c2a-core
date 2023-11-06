#!/bin/bash
echo "TlmCmd -> tlm_cmd rename migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
echo "rename c2a-core ref: src_core/TlmCmd -> src_core/tlm_cmd"
find_all | xargs -0 sed -i -e "s#src_core/TlmCmd/Ccsds#src_core/tlm_cmd/ccsds#g"
find_all | xargs -0 sed -i -e "s#src_core\\\TlmCmd\\\Ccsds#src_core\\\tlm_cmd\\\ccsds#g"

find_all | xargs -0 sed -i -e "s#src_core/TlmCmd#src_core/tlm_cmd#g"
find_all | xargs -0 sed -i -e "s#src_core\\\TlmCmd#src_core\\\tlm_cmd#g"

find_all | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/TlmCmd#\${C2A_CORE_DIR}/applications#g"

# C2A user code
echo "rename dir: src_user/TlmCmd -> src_user/tlm_cmd"
mv src/src_user/TlmCmd src/src_user/tlm_cmd

mv src/src_user/tlm_cmd/Ccsds src/src_user/tlm_cmd/ccsds
mv src/src_user/tlm_cmd/NormalBlockCommandDefinition src/src_user/tlm_cmd/normal_block_command_definition

echo "rename ref: TlmCmd -> tlm_cmd"
find_all | xargs -0 sed -i -e "s#TlmCmd/Ccsds#tlm_cmd/ccsds#g"
find_all | xargs -0 sed -i -e "s#TlmCmd\\\Ccsds#tlm_cmd\\\ccsds#g"

find_all | xargs -0 sed -i -e "s#TlmCmd/DataBase#tlm_cmd/data_base#g"
find_all | xargs -0 sed -i -e "s#TlmCmd\\\DataBase#tlm_cmd\\\data_base#g"

find_all | xargs -0 sed -i -e "s#/TlmCmd#/tlm_cmd#g"
find_all | xargs -0 sed -i -e "s#\\\TlmCmd#\\\tlm_cmd#g"

find_all | xargs -0 sed -i -e "s#TlmCmd/#tlm_cmd/#g"
find_all | xargs -0 sed -i -e "s#TlmCmd\\\#tlm_cmd\\\#g"

# CMake
echo "rename ref: TlmCmd/*"
find_all | xargs -0 sed -i -e "s#Ccsds/#ccsds/#g"
find_all | xargs -0 sed -i -e "s#NormalBlockCommandDefinition/#normal_block_command_definition/#g"

# Settings
echo "rename settings dir: Settings/TlmCmd -> Settings/tlm_cmd"
mv src/src_user/Settings/TlmCmd src/src_user/Settings/tlm_cmd

mv src/src_user/Settings/tlm_cmd/Ccsds src/src_user/Settings/tlm_cmd/ccsds
mv src/src_user/Settings/tlm_cmd/NormalBlockCommandDefinition src/src_user/Settings/tlm_cmd/normal_block_command_definition
mv src/src_user/Settings/tlm_cmd/DataBase src/src_user/Settings/tlm_cmd/data_base

# Test
echo "rename test dir: Test/test/*/TlmCmd -> Test/test/*/tlm_cmd"
mv src/src_user/Test/test/src_core/TlmCmd src/src_user/Test/test/src_core/tlm_cmd

mv src/src_user/Test/test/src_user/TlmCmd src/src_user/Test/test/src_user/tlm_cmd

# Script
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#TlmCmd#tlm_cmd#g"
