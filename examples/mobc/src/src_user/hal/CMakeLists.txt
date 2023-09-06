cmake_minimum_required(VERSION 3.13)

project(C2A_USER_HAL)

set(C2A_COMMON_SRCS
  if_list.c
)

# 通常，S2EではC++ビルドされるが，C2A core開発のため，C2A単体をC89でライブラリビルドする場合は，mockupをビルド対象にする
if(USE_SILS_MOCKUP)
  message("USE SILS_MOCKUP")

  #target_sources(${PROJECT_NAME} PRIVATE
  set(C2A_HAL_IMPL_SRCS
    sils_mockup/ccsds_sils.c
    sils_mockup/i2c_sils.c
    sils_mockup/uart_sils.c
    sils_mockup/wdt_sils.c
  )
else()
  #target_sources(${PROJECT_NAME} PUBLIC
  set(C2A_HAL_IMPL_COMMON_SRCS
    sils/ccsds_sils.cpp
    sils/i2c_sils.cpp
    sils/uart_sils.cpp
    sils/wdt_sils.cpp
  )

  if(USE_SCI_COM_WINGS)
    add_definitions(-DUSE_SCI_COM_WINGS)
    #target_sources(${PROJECT_NAME} PUBLIC
    set(C2A_HAL_COM_WINGS_SRCS
      sils/ccsds_sils_sci_if.cpp
    )
    message("USE SCI_COM_WINGS")
  endif()

  if(USE_SCI_COM_UART)
    add_definitions(-DUSE_SCI_COM_UART)
    #target_sources(${PROJECT_NAME} PUBLIC
    set(C2A_HAL_COM_UART_SRCS
      sils/uart_sils_sci_if.cpp
    )
    message("USE SCI_COM_UART")
  endif()

  set(C2A_HAL_IMPL_SRCS
    ${C2A_HAL_IMPL_COMMON_SRCS}
    ${C2A_HAL_COM_WINGS_SRCS}
    ${C2A_HAL_COM_UART_SRCS}
  )
endif()

set(C2A_SRCS
  ${C2A_COMMON_SRCS}
  ${C2A_HAL_IMPL_SRCS}
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_USER_DIR}/common.cmake)
