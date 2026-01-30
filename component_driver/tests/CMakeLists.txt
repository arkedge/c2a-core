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

set(MOCK_PRELUDE_FLAG "-include ${CMAKE_CURRENT_SOURCE_DIR}/mocks/driver_super_mock_prelude.h")

# StreamRecBuffer のテスト
add_executable(test_stream_rec_buffer
  test_stream_rec_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../driver_super.c
  mocks/mock_hal_handler_registry.c
  mocks/mock_time_manager.c
)

# driver_super.c にモック prelude を適用（test_stream_rec_buffer 用）
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/../driver_super.c
  TARGET_DIRECTORY test_stream_rec_buffer
  PROPERTIES COMPILE_FLAGS "${MOCK_PRELUDE_FLAG}"
)

target_include_directories(test_stream_rec_buffer PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks           # モックが実際のヘッダより優先される
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks/src_user  # <src_user/...> インクルード用
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(test_stream_rec_buffer
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_stream_rec_buffer)

# FrameAnalysis のテスト
add_executable(test_frame_analysis
  test_frame_analysis.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../driver_super.c
  mocks/mock_hal_handler_registry.c
  mocks/mock_time_manager.c
)

# driver_super.c と test_frame_analysis.cpp にモック prelude を適用
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/../driver_super.c
  ${CMAKE_CURRENT_SOURCE_DIR}/test_frame_analysis.cpp
  TARGET_DIRECTORY test_frame_analysis
  PROPERTIES COMPILE_FLAGS "${MOCK_PRELUDE_FLAG}"
)

target_include_directories(test_frame_analysis PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks           # モックが実際のヘッダより優先される
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks/src_user  # <src_user/...> インクルード用
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(test_frame_analysis
  GTest::gtest_main
)

gtest_discover_tests(test_frame_analysis)
