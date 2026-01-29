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

# StreamRecBuffer のテスト
add_executable(test_stream_rec_buffer
  test_stream_rec_buffer.cpp
  mocks/mock_hal_handler_registry.c
  mocks/mock_time_manager.c
)

target_include_directories(test_stream_rec_buffer PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(test_stream_rec_buffer
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_stream_rec_buffer)
