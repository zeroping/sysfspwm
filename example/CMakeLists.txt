cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_executable(pwm_control src/pwm_control.cpp)
add_dependencies(pwm_control sysfspwm)

include_directories(${PROJECT_BINARY_DIR}/../lib/src/)


target_link_libraries(pwm_control sysfspwm)
