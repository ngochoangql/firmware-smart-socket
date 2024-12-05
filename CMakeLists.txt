cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-value")
set(EXTRA_COMPONENT_DIRS components/esp-idf-lib/components)
project(smart-socket)
