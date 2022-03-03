cmake_minimum_required(VERSION 3.19)
project(terminating_threads_in_WinAPI)

set(CMAKE_CXX_STANDARD 14)

add_executable(terminating_threads_in_WinAPI main.cpp src/threads.cpp include/threads.h)
target_compile_options(terminating_threads_in_WinAPI PUBLIC -Wall -Wextra -Wpedantic)