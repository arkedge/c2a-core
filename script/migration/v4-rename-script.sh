#!/bin/bash
echo "Script -> script rename migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
echo "rename c2a-core ref: src_core/Script -> src_core/script"
find_all | xargs -0 sed -i -e "s#src_core/Script/AddSection#src_core/add_section#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Script\\\AddSection#src_core\\\script\\\add_section#g"

find_all | xargs -0 sed -i -e "s#src_core/Script/CI#src_core/script/ci#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Script\\\CI#src_core\\\script\\\ci#g"

find_all | xargs -0 sed -i -e "s#src_core/Script/Git#src_core/script/git#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Script\\\Git#src_core\\\script\\\git#g"

find_all | xargs -0 sed -i -e "s#src_core/Script#src_core/script#g"
find_all | xargs -0 sed -i -e "s#src_core\\\Script#src_core\\\script#g"

# C2A user code
echo "rename dir: src_user/Script -> src_user/script"
mv src/src_user/Script src/src_user/script

mv src/src_user/script/AddSection src/src_user/script/add_section
mv src/src_user/script/CI src/src_user/script/ci
mv src/src_user/script/Git src/src_user/script/git

echo "rename ref: src_user/Script -> src_user/script"
find_all | xargs -0 sed -i -e "s#src_user/Script/AddSection#src_user/script/add_section#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Script\\\AddSection#src_user\\\script\\\add_section#g"

find_all | xargs -0 sed -i -e "s#src_user/Script/CI#src_user/script/ci#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Script\\\CI#src_user\\\script\\\ci#g"

find_all | xargs -0 sed -i -e "s#src_user/Script/Git#src_user/script/git#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Script\\\Git#src_user\\\script\\\git#g"

find_all | xargs -0 sed -i -e "s#src_user/Script#src_user/script#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Script#src_user\\\script#g"
