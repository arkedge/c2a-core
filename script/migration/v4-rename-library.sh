#!/bin/bash
# Script -> script

## c2a-core ref
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core/Library#src_core/library#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_core\\\Library#src_core\\\library#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/Library#\${C2A_CORE_DIR}/library#g"

# C2A user code
mv src/src_user/Library src/src_user/library

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#/Library#/library#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\\\Library#\\\library#g"

# CMake
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#\${C2A_USER_DIR}/Library#\${C2A_USER_DIR}/library#g"

# Script
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Library#library#g"

git restore Doxyfile
