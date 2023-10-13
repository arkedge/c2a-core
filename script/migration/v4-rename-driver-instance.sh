#!/bin/bash
# driver instances -> component service

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
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#driver_instances#component_service#g"

echo "rename file path: di_* -> csrv_*"
for di_src in $di_srcs; do
  echo "  $di_src -> csrv_${di_src#di_}"
  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#$di_src#csrv_${di_src#di_}#g"
done

echo "rename source: DI_ -> CSRV_"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#DI_#CSRV_#g"

git restore src/src_user/Settings/tlm_cmd/data_base/CMD_DB/*.xlsm
git restore src/src_user/Settings/tlm_cmd/data_base/TLM_DB/*.xlsm
