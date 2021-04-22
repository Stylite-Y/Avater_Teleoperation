cmake_minimum_required(VERSION 3.5)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold "${Esc}[1m")
  set(Red "${Esc}[31m")
  set(Green "${Esc}[32m")
  set(Yellow "${Esc}[33m")
  set(Blue "${Esc}[34m")
  set(Magenta "${Esc}[35m")
  set(Cyan "${Esc}[36m")
  set(White "${Esc}[37m")
  set(BoldRed "${Esc}[1;31m")
  set(BoldGreen "${Esc}[1;32m")
  set(BoldYellow "${Esc}[1;33m")
  set(BoldBlue "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan "${Esc}[1;36m")
  set(BoldWhite "${Esc}[1;37m")
endif()

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_COLOR_MAKEFILE ON)
# execute_process(COMMAND ../scripts/make_types.sh)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# set(CMAKE_VERBOSE_MAKEFILE ON)

option(ROBOT_BUILD "use compiler flags for real robot computer" OFF)

set(BUILD_TYPE_RELEASE TRUE)

# if (REMOTE) SET(REMOTE_CONTROL true) message(WARNING "${BoldCyan}Enable remote
# controller, please plug in receiver.${ColourReset}") message(WARNING
# "${BoldCyan}LCM will NOT send data back !!!.${ColourReset}") else (REMOTE)
# SET(REMOTE_CONTROL false) message(WARNING "${BoldCyan}Disable remote
# controller, receiver is not needed${ColourReset}") message(WARNING
# "${BoldCyan}LCM will send data back !!!.${ColourReset}") endif(REMOTE)

if(ROBOT_BUILD)
    set(THIS_COM "../")
    set(IS_SIM false)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
                   ${CMAKE_BINARY_DIR}/Configuration.h)
    set(CMAKE_CXX_FLAGS
        "-O3 -no-pie -ggdb -Wall \
        -Wextra -Wcast-align -Wdisabled-optimization -Wformat=2 \
        -Winit-self -Wmissing-include-dirs -Woverloaded-virtual \
        -Wsign-promo -Werror -faligned-new")
    set(CMAKE_C_FLAGS "-O3  -ggdb -std=gnu99 -I.")
    message(WARNING "${BoldCyan}set to mini cheetah build${ColourReset}")
else(ROBOT_BUILD)
    set(THIS_COM "${PROJECT_SOURCE_DIR}/")
    set(IS_SIM true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
                   ${CMAKE_BINARY_DIR}/Configuration.h)

    set(CMAKE_CXX_FLAGS
        "-O3 -no-pie -march=native -ggdb -Wall \
        -Wextra -Wcast-align -Wdisabled-optimization -Wformat=2 \
        -Winit-self -Wmissing-include-dirs -Woverloaded-virtual \
        -Wsign-promo -Werror")
    set(CMAKE_C_FLAGS "-O3  -ggdb  -march=native -std=gnu99 -I.")
    message(
        WARNING
        "${BoldCyan}not set to mini cheetah build.  this won't work on the robot!${ColourReset}"
    )
endif(ROBOT_BUILD)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(robot)
add_subdirectory(common)
add_subdirectory(sim)
add_subdirectory(user)
add_subdirectory(third-party)
