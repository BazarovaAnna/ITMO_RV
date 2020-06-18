cmake_minimum_required (VERSION 2.6)
project (lab4)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -L/mnt/c/Users/anna/desktop/tests/lab4/lib64 -Wl,-rpath=/mnt/c/Users/anna/desktop/tests/lab4/lib64 -std=c99 -Wall -Werror -pedantic")

add_executable(peer
    banking.h
    child.c
    child.h
    common.h
    cs.c
    ipc.c
    ipc.h
    main.c
    pa2345.h
    queue.c
    queue.h
    )
target_link_libraries (peer runtime)
