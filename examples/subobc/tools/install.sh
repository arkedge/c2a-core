#!/bin/bash -Cue

export CARGO_NET_GIT_FETCH_WITH_CLI=true

cargo install --root . tmtc-c2a --git https://github.com/arkedge/gaia.git --tag v0.5.0

cargo install --root . tlmcmddb-cli    --version 0.2.0
cargo install --root . kble            --version 0.2.0
cargo install --root . kble-c2a        --version 0.2.0
cargo install --root . kble-eb90       --version 0.2.0
cargo install --root . jrsonnet        --version 0.5.0-pre9
