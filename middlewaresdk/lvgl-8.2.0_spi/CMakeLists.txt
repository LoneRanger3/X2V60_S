cmake_minimum_required(VERSION 3.12.4)

if(NOT ESP_PLATFORM)
  project(lvgl HOMEPAGE_URL https://github.com/lvgl/lvgl)
endif()

set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

### Compiler ###
if(${ARCH} STREQUAL "board8521d")
	set(CMAKE_C_COMPILER arm-xmv2-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xmv2-linux-g++)
elseif(${ARCH} STREQUAL "board650v200")
	set(CMAKE_C_COMPILER arm-xm-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xm-linux-g++)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=gnu99")

if(ESP_PLATFORM)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/esp.cmake)
elseif(ZEPHYR_BASE)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/zephyr.cmake)
elseif(MICROPY_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/micropython.cmake)
else()
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/custom.cmake)
endif()
