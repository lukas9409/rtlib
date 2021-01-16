set(LIB_SOURCES
    for_tests/list.c
    for_tests/vector.c
    for_tests/heap.c
    for_tests/object_pool.c
    protocols/publish_subscribe/pubsub_prot.cpp
    protocols/publish_subscribe/common.c)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Werror -O3 -fno-exceptions")
set(CMAKE_C_FLAGS "-Wall -Werror -O3")

add_library(rtlib ${LIB_SOURCES})