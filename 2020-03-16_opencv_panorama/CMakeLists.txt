cmake_minimum_required(VERSION 2.8)
project(cvtask)

set(CMAKE_CXX_STANDARD 11)

add_executable(cvtask main.cpp harris.cpp harris.h descriptor.cpp descriptor.h match.cpp match.h ransac.cpp ransac.h stitch.cpp stitch.h)

find_package(OpenCV REQUIRED)

target_link_libraries(cvtask ${OpenCV_LIBS})