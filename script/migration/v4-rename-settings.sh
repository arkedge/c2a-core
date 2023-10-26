#!/bin/bash
# rename Settings directory

echo "rename src/src_user/Settings -> src/src_user/settings"
mv src/src_user/Settings src/src_user/settings
sed -i -e "s#add_subdirectory(\${C2A_USER_DIR}/Settings)#add_subdirectory(\${C2A_USER_DIR}/settings)#g" CMakeLists.txt


echo "                   /settings/Modes -> modes"
mv src/src_user/settings/Modes src/src_user/settings/modes
sed -i -e "s#Modes/#modes/#g" src/src_user/settings/CMakeLists.txt


echo "                            /modes/TaskLists -> task_lists"
sed -i -e "s#modes/TaskLists/#modes/task_lists/#g" src/src_user/settings/CMakeLists.txt

tl_org=("Anomaly" "Elements" "GroundTest" "Composition")
tl_new=("anomaly" "elements" "ground_test" "composition")
mv src/src_user/settings/modes/TaskLists src/src_user/settings/modes/task_lists
for ((i=0; i<${#tl_org[@]}; i++)); do
  echo "                                  /task_lists/${tl_org[i]} -> ${tl_new[i]}"

  sed -i -e "s#modes/task_lists/${tl_org[i]}#modes/task_lists/${tl_new[i]}#g" src/src_user/settings/CMakeLists.txt

  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/Modes/TaskLists/${tl_org[i]}#settings/modes/task_lists/${tl_new[i]}#g"

  if [ -e src/src_user/settings/modes/task_lists/${tl_org[i]} ]; then
    mv src/src_user/settings/modes/task_lists/${tl_org[i]} src/src_user/settings/modes/task_lists/${tl_new[i]}
  fi

done
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/Modes/TaskLists#settings/modes/task_lists#g"


echo "                            /modes/Transitions -> transitions"
sed -i -e "s#modes/Transitions/#modes/transitions/#g" src/src_user/settings/CMakeLists.txt

tr_org=("Anomaly" "GroundTest")
tr_new=("anomaly" "ground_test")
mv src/src_user/settings/modes/Transitions src/src_user/settings/modes/transitions
for ((i=0; i<${#tr_org[@]}; i++)); do
  echo "                                  /transitions/${tr_org[i]} -> ${tr_new[i]}"

  find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/Modes/Transitions/${tr_org[i]}#settings/modes/transitions/${tr_new[i]}#g"

  if [ -e src/src_user/settings/modes/transitions/${tr_org[i]} ]; then
    mv src/src_user/settings/modes/transitions/${tr_org[i]} src/src_user/settings/modes/transitions/${tr_new[i]}
  fi
done
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/Modes/Transitions#settings/modes/transitions#g"

# ref
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/Modes#settings/modes#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/AnomalyHandlerRules#settings/anomaly_handler_rules#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/AnomalyLogger#settings/anomaly_logger#g"

find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#src_user/Settings/#src_user/settings/#g"
find . -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#Settings/#settings/#g"
