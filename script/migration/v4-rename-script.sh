#!/bin/bash
# Script -> script

## c2a-core ref
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Script/AddSection#src_core/add_section#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Script\\\AddSection#src_core\\\script\\\add_section#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Script/CI#src_core/script/ci#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Script\\\CI#src_core\\\script\\\ci#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Script/Git#src_core/script/git#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Script\\\Git#src_core\\\script\\\git#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Script#src_core/script#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Script#src_core\\\script#g"

# C2A user code
mv src/src_user/Script src/src_user/script

mv src/src_user/script/AddSection src/src_user/script/add_section
mv src/src_user/script/CI src/src_user/script/ci
mv src/src_user/script/Git src/src_user/script/git

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/Script/AddSection#src_user/script/add_section#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user\\\Script\\\AddSection#src_user\\\script\\\add_section#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/Script/CI#src_user/script/ci#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user\\\Script\\\CI#src_user\\\script\\\ci#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/Script/Git#src_user/script/git#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user\\\Script\\\Git#src_user\\\script\\\git#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/Script#src_user/script#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user\\\Script#src_user\\\script#g"
