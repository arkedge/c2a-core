#!/bin/bash

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

echo "IfWrapper -> hal"
echo ""

## c2a-core ref
echo "rename c2a-core ref: src_core/IfWrapper -> src_core/hal"
find_all | xargs -0 sed -i -e "s#src_core/IfWrapper/Common/i2c_common#src_core/hal/i2c_common#g"
find_all | xargs -0 sed -i -e "s#src_core/IfWrapper#src_core/hal#g"

## c2a user code
echo "rename src_user dir: src_user/IfWrapper -> src_user/hal"
mv src/src_user/IfWrapper src/src_user/hal
mv src/src_user/hal/Common src/src_user/hal/common
mv src/src_user/hal/Sils src/src_user/hal/sils
mv src/src_user/hal/SilsMockup src/src_user/hal/sils_mockup

### ref
echo "rename src_user ref: IfWrapper/*"
find_all | xargs -0 sed -i -e "s#IfWrapper/Common#hal/common#g"
find_all | xargs -0 sed -i -e "s#IfWrapper/Sils#IfWrapper/sils#g"

find_all | xargs -0 sed -i -e "s#SilsMockup#sils_mockup#g"

#### IfWrapper internal ref
echo "rename src_user ref in src/src_user/hal: IfWrapper/*"
find src/src_user/hal -name "*" -type f -print0 | xargs -0 sed -i -e "s#Common#common#g"

#### main ref
echo "rename IfWrapper -> hal"
find_all | xargs -0 sed -i -e "s#IfWrapper#hal#g"
find_all | xargs -0 sed -i -e "s#IF wrapper#HAL#g"
find_all | xargs -0 sed -i -e "s#IF_wrapper#HAL#g"
find_all | xargs -0 sed -i -e "s#IF Wrapper#HAL#g"
find_all | xargs -0 sed -i -e "s#IF_WRAPPER#HAL#g"

find_all | xargs -0 sed -i -e "s#C2A_IF#C2A_HAL#g"
find_all | xargs -0 sed -i -e "s#IF_IMPL#HAL_IMPL#g"
