cmake_minimum_required(VERSION 3.23)
project(bluesky C)

set(CMAKE_C_STANDARD 17)

add_executable(bluesky main.c arguments.h meta.h raw_files.h constants.h arguments.c raw_files.c meta.c)
