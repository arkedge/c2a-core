#!/bin/bash
echo "driver instances -> Component Service migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

# C2A user code
echo "rename C2A user code: driver_instances -> component_service"

echo "rename dir: src/src_user/applications/driver_instances -> src/src_user/applications/component_service"
mv src/src_user/applications/driver_instances src/src_user/applications/component_service

di_srcs=$(ls src/src_user/applications/component_service)

echo "rename files: di_* -> csrv_*"
for di_src in $di_srcs; do
  echo "  $di_src -> csrv_${di_src#di_}"
  mv src/src_user/applications/component_service/$di_src src/src_user/applications/component_service/csrv_${di_src#di_}
done

echo "rename path: driver_instances -> component_service"
find_all | xargs -0 sed -i -e "s#driver_instances#component_service#g"

echo "rename file path: di_* -> csrv_*"
for di_src in $di_srcs; do
  echo "  $di_src -> csrv_${di_src#di_}"
  find_all | xargs -0 sed -i -e "s#$di_src#csrv_${di_src#di_}#g"
done

echo "rename source: DI_ -> CSRV_"
find_all | xargs -0 sed -i -e "s#DI_#CSRV_#g"
