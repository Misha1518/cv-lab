cmake_minimum_required(VERSION 3.13)
project(histogram)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(hist.out main.cpp)
target_link_libraries(hist.out ${OpenCV_LIBS})
add_custom_target(run
    COMMAND hist.out
    DEPENDS hist.out
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
