#!/bin/bash
# driver instances -> Component Service

## c2a-core ref
header_org=("common_tlm_cmd_packet_for_driver_super.h" "eb90_frame_for_driver_super.h" "eb90_packet_for_driver_super.h")
header_new=("cdrv_common_tlm_cmd_packet.h" "cdrv_eb90_frame.h" "cdrv_eb90_packet.h")

echo "rename c2a-core header file name prefix & remove _for_driver_super suffix"
for i in ${!header_org[@]}; do
  echo "  ${header_org[$i]} -> ${header_new[$i]}"
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#${header_org[$i]}#${header_new[$i]}#g"
done

echo "rename init func"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#CTCP_init_cdssc(#CDRV_CTCP_init_stream_config(#g"

remove_suffix=("_from_cdssc" "_to_cdssc" "_of_cdssc" "_cdssc")
prefix_org=("CTCP_" "CTP_" "CCP_" "EB90_PACKET_" "EB90_FRAME_")

for i in ${!remove_suffix[@]}; do
  echo "remove suffix: ${remove_suffix[$i]}"
  for k in ${!prefix_org[@]}; do
    echo "   rename function: ${prefix_org[$k]}***${remove_suffix[$i]}(*** -> CDRV_${prefix_org[$k]}***(***"
    find . -name "*" -not -path "*/.git/*" -type f -print0 \
      | xargs -0 sed -i -E "s#${prefix_org[$k]}([0-9a-zA-Z_]*)${remove_suffix[$i]}\((.*)#CDRV_${prefix_org[$k]}\1\(\2#g"
  done
done

echo "rename constants"
constants=("EB90_PACKET_HEADER_SIZE" "EB90_FRAME_(MACRO_ANY)_SIZE" "EB90_FRAME_(MACRO_ANY)_BYTE")
for i in ${!constants[@]}; do
  match_org=$(echo "${constants[$i]}" | sed -e "s#(MACRO_ANY)#(\[0-9A-Z_\]\*)#g")
  match_new=$(echo "${constants[$i]}" | sed -e "s#(MACRO_ANY)#\\\1#g")

  echo "  ${match_org} -> CDRV_${match_new}"
  find . -name "*" -not -path "*/.git/*" -type f -print0 \
    | xargs -0 sed -i -E "s#${match_org}#CDRV_${match_new}#g"
done

echo "rename globals"
global=("EB90_FRAME_kStx" "EB90_FRAME_kEtx")
for i in ${!global[@]}; do
  echo "  ${global[$i]} -> CDRV_${global[$i]}"
  find . -name "*" -not -path "*/.git/*" -type f -print0 \
    | xargs -0 sed -i -E "s#${global[$i]}#CDRV_${global[$i]}#g"
done

git restore src/src_user/Settings/tlm_cmd/data_base/CMD_DB/*.xlsm
git restore src/src_user/Settings/tlm_cmd/data_base/TLM_DB/*.xlsm
