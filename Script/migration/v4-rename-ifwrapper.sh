#!/bin/bash

# IfWrapper -> hal
## c2a-core ref
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/IfWrapper/Common/i2c_common#src_core/hal/i2c_common#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/IfWrapper#src_core/hal#g"
## c2a user code
mkdir -p src/src_user/hal
mv src/src_user/IfWrapper/Common src/src_user/hal/common
mv src/src_user/IfWrapper/Sils src/src_user/hal/sils
mv src/src_user/IfWrapper/SilsMockup src/src_user/hal/sils_mockup
mv src/src_user/IfWrapper/* src/src_user/hal/

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/IfWrapper/Common#src_user/hal/common#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#SilsMockup#sils_mockup#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Sils#sils#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IfWrapper#hal#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IF wrapper#HAL#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IF_wrapper#HAL#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IF Wrapper#HAL#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IF_WRAPPER#HAL#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#C2A_IF#C2A_HAL#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#IF_IMPL#HAL_IMPL#g"
