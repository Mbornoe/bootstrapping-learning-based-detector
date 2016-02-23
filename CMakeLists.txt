cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bootstrappingForLearningBasedDetecotr)
find_package( OpenCV REQUIRED )

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

add_executable(bootstrapping bootstrapping.cpp bootstrapping.hpp)
target_link_libraries(bootstrapping ${OpenCV_LIBS})

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    #-fopenmp
    # Other flags
)
