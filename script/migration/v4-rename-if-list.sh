#!/bin/bash
echo "IF list rename migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## IF list -> DS handler registry
echo "move if_list source files to src/src_user/Settings/component_driver_super"
mv src/src_user/hal/if_list.h src/src_user/Settings/component_driver_super/hal_handler_registry.h
mv src/src_user/hal/if_list.c src/src_user/Settings/component_driver_super/hal_handler_registry.c

## include
### guard
sed -i -e "s#IF_LIST_H_#HAL_HANDLER_REGISTRY_H_#g" src/src_user/Settings/component_driver_super/hal_handler_registry.h

### ref
find_all | xargs -0 sed -i -e "s#src_user/hal/if_list\.h#src_user/Settings/component_driver_super/hal_handler_registry\.h#g"
find_all | xargs -0 sed -i -e "s#src_user\\\hal\\\if_list\.h#src_user\\\Settings\\\component_driver_super\\\hal_handler_registry\.h#g"
find_all | xargs -0 sed -i -e "s#src_user\\\hal\\\if_list\.c#src_user\\\Settings\\\component_driver_super\\\hal_handler_registry\.c#g"

sed -i -e "s#\"if_list\.h\"#\"hal_handler_registry\.h\"#g" src/src_user/Settings/component_driver_super/hal_handler_registry.c

## CMake
echo "remove src/src_user/hal/if_list.c from CMakeLists.txt"
sed -i -e "s#if_list\.c##g" src/src_user/hal/CMakeLists.txt
sed -i -e "s#src/src_user/hal/if_list\.c##g" CMakeLists.txt

echo "add src/src_user/Settings/component_driver_super/hal_handler_registry.c to CMakeLists.txt"
sed -i -z -e "s#set(C2A_SRCS\n#set(C2A_SRCS\n  component_driver_super/hal_handler_registry\.c\n#g" src/src_user/Settings/CMakeLists.txt

## hal handler list
echo "rename handler list"
func_org=("IF_init" "IF_RX" "IF_TX" "IF_reopen")
func_new=("HAL_init_handlers" "HAL_rx_handlers" "HAL_tx_handlers" "HAL_reopen_handlers")

for i in ${!func_org[@]}; do
  echo "  ${func_org[$i]}[ -> ${func_new[$i]}["
  find_all | xargs -0 sed -i -e "s#${func_org[$i]}\[#${func_new[$i]}\[#g"

  ### for comment
  echo "  ${func_org[$i]} -> ${func_new[$i]}["
  find_all | xargs -0 sed -i -e "s# ${func_org[$i]} # ${func_new[$i]} #g"
  find_all | xargs -0 sed -i -e "s#${func_org[$i]} #${func_new[$i]} #g"
  find_all | xargs -0 sed -i -e "s# ${func_org[$i]}# ${func_new[$i]}#g"
done

## hal handler config
echo "p_super->interface -> p_super->hal_handler_id"
#find_all | xargs -0 sed -i -e "s#IF_LIST_ENUM(\s)interface;#IF_LIST_ENUM\\\1hal_handler_id;#g"
find_all | xargs -0 sed -i -e "s#p_super->interface#p_super->hal_handler_id#g"

echo "p_super->if_config -> p_super->hal_config"
find_all | xargs -0 sed -i -e "s#if_config#hal_config#g"

## other members
echo "ret_from_if_rx -> ret_from_hal_rx"
find_all | xargs -0 sed -i -e "s#ret_from_if_rx#ret_from_hal_rx#g"
find_all | xargs -0 sed -i -e "s#RET_FROM_IF_RX#RET_FROM_HAL_RX#g"

echo "ret_from_if_tx -> ret_from_hal_tx"
find_all | xargs -0 sed -i -e "s#ret_from_if_tx#ret_from_hal_tx#g"
find_all | xargs -0 sed -i -e "s#RET_FROM_IF_TX#RET_FROM_HAL_TX#g"


## enum
enum_org=("IF_LIST_ENUM" "IF_LIST_MAX" "IF_REOPEN_REASON")
enum_new=("HAL_HANDLER_ID" "HAL_HANDLER_ID_MAX" "HAL_HANDLER_REOPEN_REASON")

echo "rename enum"
for i in ${!enum_org[@]}; do
  echo "  ${enum_org[$i]} -> ${enum_new[$i]}"
  find_all | xargs -0 sed -i -e "s#${enum_org[$i]}#${enum_new[$i]}#g"
done

## buffer
echo "rename buffers"
buf_org=("CDS_if_rx_buffer_" "CDS_IF_RX_BUFFER_SIZE" "rx_buffer_size_in_if_rx")
buf_new=("CDS_hal_rx_buffer_" "CDS_HAL_RX_BUFFER_SIZE" "hal_rx_buffer_size")

for i in ${!buf_org[@]}; do
  echo "  ${buf_org[$i]} -> ${buf_new[$i]}"
  find_all | xargs -0 sed -i -e "s#${buf_org[$i]}#${buf_new[$i]}#g"
done
