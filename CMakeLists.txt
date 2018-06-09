cmake_minimum_required(VERSION 3.0)

project(FanController)

#Build options
    #Set release to default build type
set(CMAKE_BUILD_TYPE 'Release')
message(STATUS "Build type = ${CMAKE_BUILD_TYPE}")

    #Option to print discovered locations of dependencies
option(DEP_LOCATIONS OFF)

    #Option to use existing dependency installs, in default locations
option(DEP_USE_EXISITING OFF)

#Target executable defined in this sub-directory
add_subdirectory(sources)
