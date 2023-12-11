#!/bin/bash
echo "driver -> component driver migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
echo "rename c2a-core ref: driver -> component_driver"
find_all | xargs -0 sed -i -e "s#src_core/driver#src_core/component_driver#g"
find_all | xargs -0 sed -i -e "s#src_core\\\driver#src_core\\\component_driver#g"

find_all | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/driver#\${C2A_CORE_DIR}/component_driver#g"

# C2A user code
echo "rename dir: driver -> component_driver"
mv src/src_user/driver src/src_user/component_driver
mv src/src_user/Settings/DriverSuper src/src_user/Settings/component_driver_super

find_all | xargs -0 sed -i -e "s#\${C2A_USER_DIR}/driver#\${C2A_USER_DIR}/component_driver#g"
find_all | xargs -0 sed -i -e "s#/driver/#/component_driver/#g"
find_all | xargs -0 sed -i -e "s#\\\driver\\\#\\\component_driver\\\#g"

## _init_dssc(), _from_dssc(), _to_dssc()
find_all | xargs -0 sed -i "s/dssc/cdssc/g"

## Settings
find_all | xargs -0 sed -i -e "s#/DriverSuper/#/component_driver_super/#g"

# Script
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#drivers#component_driver#g"

echo "DriverSuper -> ComponentDriverSuper"
find_all | xargs -0 sed -i -e "s#DriverSuper#ComponentDriverSuper#g"

## prefix
prefix_org=("DS_" "DSC_" "DSSC_")
prefix_new=("CDS_" "CDSC_" "CDSSC_")
echo "rename prefix:"
for ((i=0; i<${#prefix_org[@]}; i++)); do
  echo "  ${prefix_org[i]} -> ${prefix_new[i]}"
  find_all | xargs -0 sed -i "/CCSDS_/!s/${prefix_org[i]}/${prefix_new[i]}/g"
done
