#!/bin/bash
# DriverSuper -> Framing

## c2a-core ref
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers/Super#src_core/framing#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers\\\Super#src_core\\\framing#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers/Protocol#src_core/framing#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers\\\Protocol#src_core\\\framing#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers#src_core/framing#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers#src_core\\\framing#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/Drivers#\${C2A_CORE_DIR}/framing#g"

## c2a-core header
headers_org=("driver_super.h" "common_tlm_cmd_packet_for_driver_super.h" "eb90_frame_for_driver_super.h" "eb90_packet_for_driver_super.h")
headers_new=("framing.h" "framing_common_tlm_cmd_packet.h" "framing_eb90_frame.h" "framing_eb90_packet.h")

for ((i=0;i<${#headers_org[@]};++i)); do
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#framing/${headers_org[$i]}#framing/${headers_new[$i]}#g"
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#framing\\\"${headers_org[$i]}#framing\\\"${headers_new[$i]}#g"
done

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#framing\driver_super#src_core/framing#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#framing\\\#src_core\\\framing#g"

## prefix
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i '/CCSDS_/!s/DS_/FRM_/g'
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i 's/DSSC_/FRMSC_/g'

# Settings
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/DriverSuper#Settings/framing#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings\\\DriverSuper#Settings\\\framing#g"

mv src/src_user/Settings/DriverSuper src/src_user/Settings/framing

settings_org=("driver_buffer_define.h" "driver_super_params.h")
settings_new=("framing_buffer_define.h" "framing_params.h")

for ((i=0;i<${#settings_org[@]};++i)); do
  mv src/src_user/Settings/framing/${settings_org[$i]} src/src_user/Settings/framing/${settings_new[$i]}
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/framing/${settings_org[$i]}#Settings/framing/${settings_new[$i]}#g"
  find src/src_user/Settings -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#${settings_org[$i]}#${settings_new[$i]}#g"

  # include guard
  settings_org_gurad=$(echo ${settings_org[$i]} | tr '[:lower:]' '[:upper:]' | sed 's/\./_/g')
  settings_new_gurad=$(echo ${settings_new[$i]} | tr '[:lower:]' '[:upper:]' | sed 's/\./_/g')

  sed -i "s/${settings_org_gurad}/${settings_new_gurad}/g" src/src_user/Settings/framing/${settings_new[$i]}
done

## type
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i 's/DriverSuper/Framing/g'

## variable
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i 's/p_super/p_framing/g'
