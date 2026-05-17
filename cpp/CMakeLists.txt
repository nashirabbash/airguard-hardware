cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/version.cmake)
project(airguard-hardware-cpp LANGUAGES C CXX)

set(EXTRA_COMPONENT_DIRS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project_framework_init()

project_framework_build()
