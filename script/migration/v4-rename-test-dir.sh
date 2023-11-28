#!/bin/bash

echo "rename test top dir src/src_user/Test -> src/src_user/test"
mv src/src_user/Test src/src_user/test

echo "move test case files to src/src_user/test"
mv src/src_user/test/test/* src/src_user/test

echo "remove test dir src/src_user/test/test"
rm -r src/src_user/test/test

sed -i 's#ROOT_PATH = "../"#ROOT_PATH = "./"#' src/src_user/test/conftest.py
