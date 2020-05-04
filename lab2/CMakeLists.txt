cmake_minimum_required (VERSION 2.6)
project (lab2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -L/mnt/c/Users/anna/desktop/tests/lab2/lib64 -std=c99 -Wall -Werror -pedantic")

add_executable(peer
    banking.h
    bank_robbery.c
    child.c
    child.h
    common.h
    ipc.c
    ipc.h
    main.c
    pa2345.h
    parent.c
    parent.h
    var_lib.h
        logwriter.c logwriter.h)
target_link_libraries (peer runtime)
