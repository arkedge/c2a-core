#!/bin/bash -Cue

export BINSTALL_VERSION="v1.10.14"
export CARGO_NET_GIT_FETCH_WITH_CLI=true

curl -L --proto '=https' --tlsv1.2 -sSf "https://raw.githubusercontent.com/cargo-bins/cargo-binstall/${BINSTALL_VERSION}/install-from-binstall-release.sh" | env BINSTALL_VERSION=${BINSTALL_VERSION} CARGO_HOME=$(pwd) bash

./bin/cargo-binstall --root . tmtc-c2a               --version 1.0.0 --no-confirm

cargo install --debug --root . tlmcmddb-cli    --version 0.2.0
./bin/cargo-binstall --root . kble                   --version 0.3.0 --no-confirm
./bin/cargo-binstall --root . kble-c2a               --version 0.3.0 --no-confirm
./bin/cargo-binstall --root . kble-eb90              --version 0.3.0 --no-confirm
cargo install --debug --root . jrsonnet        --version 0.5.0-pre9 --locked
