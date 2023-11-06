#!/bin/bash
echo "Script -> script migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
echo "rename c2a-core ref: src_core/Applications/* -> src_core/applications/*"
find_all | xargs -0 sed -i -e "s#src_core/Applications/TestApp#src_core/applications/test_app#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Applications\\\TestApp#src_core\\\applications\\\test_app#g"

find_all | xargs -0 sed -i -e "s#src_core/Applications#src_core/applications#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Applications#src_core\\\applications#g"

find_all | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/Applications#\${C2A_CORE_DIR}/applications#g"

# C2A user code
user_defined_org=("Aocs" "Cdh" "Com" "Etc" "Mission" "Power" "Prop" "Thermal")
user_defined_new=("aocs" "cdh" "com" "etc" "mission" "power" "prop" "thermal")

echo "rename dir: src_user/Applications/ -> src_user/applications/"
mv src/src_user/Applications src/src_user/applications

echo "rename dir: src_user/applications/*"
mv src/src_user/applications/DriverInstances src/src_user/applications/driver_instances
mv src/src_user/applications/Middleware src/src_user/applications/middleware
mv src/src_user/applications/UserDefined src/src_user/applications/user_defined

echo "rename dir: applications/user_defined/"
for ((i=0; i<${#user_defined_org[@]}; i++)); do
  echo "  ${user_defined_org[i]} -> ${user_defined_new[i]}"

  mv "src/src_user/applications/user_defined/${user_defined_org[i]}" "src/src_user/applications/user_defined/${user_defined_new[i]}"
done

echo "rename ref: Applications/*"

find_all | xargs -0 sed -i -e "s#Applications/DriverInstances#applications/driver_instances#g"
find_all | xargs -0 sed -i -e "s#Applications\\\DriverInstances#applications\\\driver_instances#g"

find_all | xargs -0 sed -i -e "s#Applications/Middleware#applications/middleware#g"
find_all | xargs -0 sed -i -e "s#Applications\\\Middleware#applications\\\middleware#g"

find_all | xargs -0 sed -i -e "s#Applications/UserDefined#applications/user_defined#g"
find_all | xargs -0 sed -i -e "s#Applications\\\UserDefined#applications\\\user_defined#g"

find_all | xargs -0 sed -i -e "s#/Applications#/applications#g"
find_all | xargs -0 sed -i -e "s#\\\Applications#\\\applications#g"

# CMake
find_all | xargs -0 sed -i -e "s#DriverInstances/#driver_instances/#g"
find_all | xargs -0 sed -i -e "s#Middleware/#middleware/#g"

echo "rename ref: UserDefined"
for ((i=0; i<${#user_defined_org[@]}; i++)); do
  echo "  ${user_defined_org[i]} -> ${user_defined_new[i]}"

  find_all | xargs -0 sed -i -e "s#UserDefined/${drv_org[i]}#user_defined/${drv_new[i]}#g"
  find_all | xargs -0 sed -i -e "s#UserDefined\\\${drv_org}#user_defined\\\${drv_new}#g"
done

find_all | xargs -0 sed -i -e "s#UserDefined/#user_defined/#g"

# Settings
echo "rename dir: src_user/Settings/Applications -> src_user/Settings/applications"
mv src/src_user/Settings/Applications src/src_user/Settings/applications

# Test
echo "rename dir: src_user/Test/test/src_core/Applications -> src_user/Test/test/src_core/applications"
mv src/src_user/Test/test/src_core/Applications src/src_user/Test/test/src_core/applications

echo "rename dir: src_user/Test/test/src_user/Applications -> src_user/Test/test/src_user/applications"
mv src/src_user/Test/test/src_user/Applications src/src_user/Test/test/src_user/applications
mv src/src_user/Test/test/src_user/Applications/DriverInstances src/src_user/Test/test/src_user/applications/driver_instances
mv src/src_user/Test/test/src_user/Applications/Middleware src/src_user/Test/test/src_user/applications/middleware
mv src/src_user/Test/test/src_user/Applications/UserDefined src/src_user/Test/test/src_user/applications/user_defined

# Script
echo "rename ref in script: Applications -> applications"
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Applications#applications#g"
