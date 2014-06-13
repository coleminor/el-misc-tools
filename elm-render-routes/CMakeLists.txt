cmake_minimum_required(VERSION 2.6)
project(elm-render-routes)

set(EXECUTABLE_NAME "elm-render-routes")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

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

configure_file(
  "${CMAKE_SOURCE_DIR}/defines.hpp.in"
  "${CMAKE_BINARY_DIR}/defines.hpp"
)
include_directories("${CMAKE_BINARY_DIR}")

find_package(CAIRO REQUIRED)
include_directories(${CAIRO_INCLUDE_DIRS})

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

set(SOURCES
  brush.cpp
  brush.hpp
  color.cpp
  color.hpp
  file.cpp
  file.hpp
  format.cpp
  format.hpp
  grid.cpp
  grid.hpp
  heuristic.cpp
  heuristic.hpp
  image.cpp
  image.hpp
  main.cpp
  options.cpp
  options.hpp
  path.cpp
  path.hpp
  point.hpp
  pool.hpp
  preprocessor.hpp
  report.cpp
  report.hpp
  route.cpp
  route.hpp
  tile.cpp
  tile.hpp
  timer.cpp
  timer.hpp
  utility.hpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_link_libraries(${EXECUTABLE_NAME}
  ${CAIRO_LIBRARIES}
)

install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION bin
)
