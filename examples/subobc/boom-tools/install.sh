#!/bin/bash -Cue

export BINSTALL_VERSION="v1.10.14"
export JRSONNET_VERSION="v0.5.0-pre96-test"

BIN_NAMES=(tmtc-c2a tlmcmddb-cli kble kble-serialport kble-c2a kble-eb90 jrsonnet)

## restore binaries from cache & skip network access entirely (for CI)
## NOTE: cache invalidation relies on the cache key hashing this script (tool versions are defined above)
if [ -n "${C2A_BOOM_TOOLS_CACHE_DIR:-}" ]; then
  cache_complete=true
  for name in "${BIN_NAMES[@]}"; do
    [ -x "${C2A_BOOM_TOOLS_CACHE_DIR}/${name}" ] || cache_complete=false
  done
  if "${cache_complete}"; then
    mkdir -p ./bin
    for name in "${BIN_NAMES[@]}"; do
      cp "${C2A_BOOM_TOOLS_CACHE_DIR}/${name}" "./bin/${name}"
    done
    echo "c2a-boom-tools: restored binaries from ${C2A_BOOM_TOOLS_CACHE_DIR}"
    exit 0
  fi
fi

curl -L --proto '=https' --tlsv1.2 -sSf "https://raw.githubusercontent.com/cargo-bins/cargo-binstall/${BINSTALL_VERSION}/install-from-binstall-release.sh" | env BINSTALL_VERSION=${BINSTALL_VERSION} CARGO_HOME=$(pwd) bash

./bin/cargo-binstall --root . tmtc-c2a               --version 1.1.1 --no-confirm

./bin/cargo-binstall --root . tlmcmddb-cli           --version 2.6.1 --no-confirm
./bin/cargo-binstall --root . kble                   --version 0.4.2 --no-confirm
./bin/cargo-binstall --root . kble-serialport        --version 0.4.2 --no-confirm
./bin/cargo-binstall --root . kble-c2a               --version 0.4.2 --no-confirm
./bin/cargo-binstall --root . kble-eb90              --version 0.4.2 --no-confirm

## install jrsonnet
arch=$(uname -m)
if [ "$arch" = "x86_64" ]; then
  arch="amd64"
fi
os=$(uname -s | tr -s '[:upper:]' '[:lower:]')
curl -L "https://github.com/CertainLach/jrsonnet/releases/download/${JRSONNET_VERSION}/jrsonnet-linux-${arch}" -o ./bin/jrsonnet
chmod +x ./bin/jrsonnet

## save binaries to cache (for CI)
if [ -n "${C2A_BOOM_TOOLS_CACHE_DIR:-}" ]; then
  mkdir -p "${C2A_BOOM_TOOLS_CACHE_DIR}"
  for name in "${BIN_NAMES[@]}"; do
    cp "./bin/${name}" "${C2A_BOOM_TOOLS_CACHE_DIR}/${name}"
  done
fi
