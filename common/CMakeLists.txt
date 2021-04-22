# common library (libbiomimetics)

#cmake_minimum_required(VERSION 2.8.2)

#project(libbiomimetics)

# Source/Include files
include_directories(".")
include_directories("include/" )                 # common includes
include_directories("/usr/local/include/lcm/")   # lcm includes
include_directories("../robot/include")
include_directories("../lcm-types/cpp/")
include_directories("../third-party/inih")       # inih includes
include_directories("../third-party/ParamHandler")
include_directories(${CMAKE_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE sources "src/*.cpp")                   # common library cpp files
file(GLOB_RECURSE test_sources "test/test_*.cpp")             # test cpp files

# Library
add_library(biomimetics SHARED ${sources})       # produce a library used by sim/robot
target_link_libraries(biomimetics inih dynacore_param_handler)

# Test
# add_executable(test-common ${test_sources} ${sources})
# target_link_libraries(test-common gtest gmock_main lcm rt inih osqp dynacore_param_handler pthread)
