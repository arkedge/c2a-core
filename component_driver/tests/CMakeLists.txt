cmake_minimum_required(VERSION 3.14)

project(component_driver_tests)

# GoogleTest を FetchContent で取得
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

set(MOCK_PRELUDE_FLAG "-include ${CMAKE_CURRENT_SOURCE_DIR}/mocks/driver_super_mock_prelude.h")

# テスト対象本体
set(DRIVER_SUPER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../driver_super.c)
# 実コア実装: 時刻演算は手書きモックではなく実物をリンクして実挙動を検証する
set(REAL_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../system/time_manager/obc_time.c)
# 真の境界 (HAL / グローバル時刻 / print) のみスタブ化する
set(MOCK_SRC
  mocks/mock_hal_handler_registry.c
  mocks/mock_time_manager.c
  mocks/mock_print.c
)

set(TEST_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks            # 各種スタブ
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks/src_user   # <src_user/...> (テスト用 config)
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../..            # <system/...> <library/...> (実コア)
)

# prelude は driver_super.c とテスト本体にのみ適用する
# (実コア obc_time.c / スタブ .c には適用しない)。
function(add_cds_test target test_src)
  add_executable(${target}
    ${test_src}
    ${DRIVER_SUPER_SRC}
    ${REAL_CORE_SRC}
    ${MOCK_SRC}
  )
  set_source_files_properties(
    ${DRIVER_SUPER_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/${test_src}
    TARGET_DIRECTORY ${target}
    PROPERTIES COMPILE_FLAGS "${MOCK_PRELUDE_FLAG}"
  )
  target_include_directories(${target} PRIVATE ${TEST_INCLUDE_DIRS})
  target_link_libraries(${target} GTest::gtest_main)
  gtest_discover_tests(${target})
endfunction()

add_cds_test(test_stream_rec_buffer test_stream_rec_buffer.cpp)
add_cds_test(test_frame_analysis test_frame_analysis.cpp)
