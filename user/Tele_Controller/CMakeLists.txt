include_directories("../../robot/include")
include_directories("../../common/include/")
include_directories("../../lcm-types/cpp")
include_directories("/usr/local/include/lcm/")   # lcm includes
include_directories(${CMAKE_BINARY_DIR})

add_executable(tele_ctrl main.cpp Tele_Controller.cpp)
target_link_libraries(tele_ctrl robot biomimetics dynacore_param_handler)