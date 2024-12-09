#!/bin/bash -Cue

export BINSTALL_VERSION="v1.10.14"
export JRSONNET_VERSION="v0.5.0-pre96-test"

curl -L --proto '=https' --tlsv1.2 -sSf "https://raw.githubusercontent.com/cargo-bins/cargo-binstall/${BINSTALL_VERSION}/install-from-binstall-release.sh" | env BINSTALL_VERSION=${BINSTALL_VERSION} CARGO_HOME=$(pwd) bash

./bin/cargo-binstall --root . tmtc-c2a               --version 1.0.0 --no-confirm

./bin/cargo-binstall --root . tlmcmddb-cli           --version 2.6.1 --no-confirm
./bin/cargo-binstall --root . kble                   --version 0.3.0 --no-confirm
./bin/cargo-binstall --root . kble-c2a               --version 0.3.0 --no-confirm
./bin/cargo-binstall --root . kble-eb90              --version 0.3.0 --no-confirm

## install jrsonnet
arch=$(uname -m)
if [ "$arch" = "x86_64" ]; then
  arch="amd64"
fi
os=$(uname -s | tr -s '[:upper:]' '[:lower:]')
curl -L "https://github.com/CertainLach/jrsonnet/releases/download/${JRSONNET_VERSION}/jrsonnet-linux-${arch}" -o ./bin/jrsonnet
chmod +x ./bin/jrsonnet
