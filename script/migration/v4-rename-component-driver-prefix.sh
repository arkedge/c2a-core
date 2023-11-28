#!/bin/bash
echo "driver instances -> Component Service migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
src_org=("common_tlm_cmd_packet_for_driver_super" "eb90_frame_for_driver_super" "eb90_packet_for_driver_super")
src_new=("cdrv_common_tlm_cmd_packet" "cdrv_eb90_frame" "cdrv_eb90_packet")

echo "rename c2a-core header file name prefix & remove _for_driver_super suffix"
for i in ${!src_org[@]}; do
  echo "  ${src_org[$i]}.h -> ${src_new[$i]}.h"
  find_all | xargs -0 sed -i -e "s#${src_org[$i]}\.h#${src_new[$i]}\.h#g"

  # rename c2a-core source impl ref for not CMake environment
  echo "  ${src_org[$i]}.c -> ${src_new[$i]}.c"
  find_all | xargs -0 sed -i -e "s#${src_org[$i]}\.c#${src_new[$i]}\.c#g"
done

echo "rename init func"
find_all | xargs -0 sed -i -e "s#CTCP_init_cdssc(#CDRV_CTCP_init_stream_config(#g"

remove_suffix=("_from_cdssc" "_to_cdssc" "_of_cdssc" "_cdssc")
prefix_org=("CTCP_" "CTP_" "CCP_" "EB90_PACKET_" "EB90_FRAME_")

for i in ${!remove_suffix[@]}; do
  echo "remove suffix: ${remove_suffix[$i]}"
  for k in ${!prefix_org[@]}; do
    echo "   rename function: ${prefix_org[$k]}***${remove_suffix[$i]}(*** -> CDRV_${prefix_org[$k]}***(***"
    find_all \
      | xargs -0 sed -i -E "s#${prefix_org[$k]}([0-9a-zA-Z_]*)${remove_suffix[$i]}\((.*)#CDRV_${prefix_org[$k]}\1\(\2#g"
  done
done

echo "rename constants"
constants=("EB90_PACKET_HEADER_SIZE" "EB90_FRAME_(MACRO_ANY)_SIZE" "EB90_FRAME_(MACRO_ANY)_BYTE")
for i in ${!constants[@]}; do
  match_org=$(echo "${constants[$i]}" | sed -e "s#(MACRO_ANY)#(\[0-9A-Z_\]\*)#g")
  match_new=$(echo "${constants[$i]}" | sed -e "s#(MACRO_ANY)#\\\1#g")

  echo "  ${match_org} -> CDRV_${match_new}"
  find_all \
    | xargs -0 sed -i -E "s#${match_org}#CDRV_${match_new}#g"
done

echo "rename globals"
global=("EB90_FRAME_kStx" "EB90_FRAME_kEtx")
for i in ${!global[@]}; do
  echo "  ${global[$i]} -> CDRV_${global[$i]}"
  find_all \
    | xargs -0 sed -i -E "s#${global[$i]}#CDRV_${global[$i]}#g"
done
