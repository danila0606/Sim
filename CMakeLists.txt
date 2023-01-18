cmake_minimum_required(VERSION 3.10)
project(Sim)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(elfio-3.11/elfio)
include_directories(elfio-3.11)

file(GLOB CPP_SOURCES
     "main.cpp"
)

add_executable(${PROJECT_NAME} ${CPP_SOURCES})