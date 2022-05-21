cmake_minimum_required(VERSION 3.23)
project(bluesky C)

set(CMAKE_C_STANDARD 17)

add_executable(bluesky src/main.c src/arguments.h src/meta.h src/raw_files.h src/constants.h src/arguments.c src/raw_files.c src/meta.c)
