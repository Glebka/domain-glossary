cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
project(glossary-server CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(project_sources
    main.cpp
    listener.cpp
    clientworker.cpp
    dataprovider.cpp
    common.cpp
)

set(project_headers
    listener.h
    clientworker.h
    dataprovider.h
    common.h
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb3 -Wall -Wextra -pedantic")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)

message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

add_executable(glossary-server
               ${project_sources}
               ${project_headers}
)

set_target_properties(glossary-server PROPERTIES AUTOMOC TRUE)
qt5_use_modules(glossary-server Core Network Xml)
