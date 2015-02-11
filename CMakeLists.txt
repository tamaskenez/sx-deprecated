cmake_minimum_required(VERSION 2.8.11.2)
project(sx)

set(CMAKE_VERBOSE_MAKEFILE 0)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()


include_directories(sx/include)

add_subdirectory(sx)

add_subdirectory(main)


