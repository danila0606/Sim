cmake_minimum_required(VERSION 3.10)
project(Sim)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
     include_directories(elfio-3.11/elfio_windows/elfio)
     include_directories(elfio-3.11/elfio_windows)
endif(WIN32)

if (UNIX)
     include_directories(elfio-3.11/elfio_linux/elfio)
     include_directories(elfio-3.11/elfio_linux)
endif(UNIX)

file(GLOB CPP_SOURCES
     "main.cpp"
     "Sim/Sim.cpp"
)

add_executable(${PROJECT_NAME} ${CPP_SOURCES})