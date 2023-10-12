#!/bin/bash
# Script -> script

## c2a-core ref
echo "rename c2a-core ref: Drivers -> driver"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers/Super#src_core/driver#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers\\\Super#src_core\\\driver#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers/Protocol#src_core/driver#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers\\\Protocol#src_core\\\driver#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Drivers#src_core/driver#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Drivers#src_core\\\driver#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/Drivers#\${C2A_CORE_DIR}/driver#g"

# C2A user code
echo "rename C2A user code: Drivers -> driver"
mv src/src_user/Drivers src/src_user/driver

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\${C2A_USER_DIR}/Drivers#\${C2A_USER_DIR}/driver#g"

drv_org=("Aocs" "Cdh" "Com" "Etc" "Mission" "Power" "Prop" "Thermal")
drv_new=("aocs" "cdh" "com" "etc" "mission" "power" "prop" "thermal")

echo "rename dir: driver/Drivers -> driver/driver"
for ((i=0; i<${#drv_org[@]}; i++)); do
  echo "  driver/${drv_org[i]} -> driver/${drv_new[i]}"
  mv src/src_user/driver/${drv_org[i]} src/src_user/driver/${drv_new[i]}
done

echo "rename ref: Drivers -> driver"
for ((i=0; i<${#drv_org[@]}; i++)); do
  echo "  Driver/${drv_org[i]} -> driver/${drv_new[i]}"

  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Drivers/${drv_org[i]}#driver/${drv_new[i]}#g"
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Drivers\\\${drv_org}#driver\\\${drv_new}#g"
done

# CMake
for ((i=0; i<${#drv_org[@]}; i++)); do
  echo "rename ref: ${drv_org[i]} -> ${drv_new[i]}"

  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#${drv_org[i]}/#${drv_new[i]}/#g"
done

# Script
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Drivers#driver#g"
