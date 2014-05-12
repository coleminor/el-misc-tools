cmake_minimum_required(VERSION 2.6)
project(elm-annotate)

set(EXECUTABLE_NAME "elm-annotate")
set(ELM_ANNOTATE_VERSION_MAJOR 0)
set(ELM_ANNOTATE_VERSION_MINOR 4)

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake"
  ${CMAKE_MODULE_PATH}
)

option(DEBUG "Debug Mode" OFF)

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Release Debug)
  set(CMAKE_CONFIGURATION_TYPES
    "${CMAKE_CONFIGURATION_TYPES}"
    CACHE STRING
    "Reset the configurations to what we need"
    FORCE
  )
endif()

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)

set(SHARE_PATH "share/${EXECUTABLE_NAME}")
set(DATA_DIR
  "${CMAKE_INSTALL_PREFIX}/${SHARE_PATH}"
)

configure_file(
  "${CMAKE_SOURCE_DIR}/source/defines.hpp.in"
  "${CMAKE_BINARY_DIR}/defines.hpp"
)
include_directories("${CMAKE_BINARY_DIR}")

find_package(SFML 2 REQUIRED system window graphics)
include_directories(${SFML_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

if(DEBUG)
  message(STATUS "Building in DEBUG mode")
  set(CMAKE_BUILD_TYPE Debug)
else()
  message(STATUS "Building in RELEASE mode")
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-fno-rtti)
  add_definitions(-ansi -pedantic)
  add_definitions(-Wall -Werror)
  add_definitions(-Wextra -Wshadow -Wstrict-aliasing)
  add_definitions(-Weffc++)
  set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb"
  )
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(source)

install(DIRECTORY resources/
  DESTINATION ${SHARE_PATH}/resources
)
