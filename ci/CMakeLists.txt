cmake_minimum_required(VERSION 3.0)

project(rtlib-ut)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_STANDARD 11)

include_directories(googletest-release-1.10.0/googletest/include)
include_directories(${CMAKE_HOME_DIRECTORY}/src)

add_subdirectory(googletest-release-1.10.0)

set(TESTED_FILES
    ../src/for_tests/list.c
    ../src/for_tests/vector.c
    ../src/for_tests/object_pool.c
    ../src/protocols/publish_subscribe/pubsub_prot.cpp)

set(TESTS
    list_test.cpp
    vector_test.cpp
    heap_test.cpp
    object_pool_test.cpp
    typed_pool_test.cpp
    queue_containers_test.cpp
    set_test.cpp
    pubsub_prot_test.cpp
    )

set(CMAKE_CXX_FLAGS "-Wall -Werror -save-temps=obj")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

add_library(tested_utils ${TESTED_FILES})

add_executable(ut ut.cpp ${TESTS})
target_link_libraries(ut gtest gtest_main gmock gmock_main tested_utils)

add_executable(performance_tests ${TESTED_FILES} performance_tests/performance_test.cpp)
target_link_libraries(performance_tests tested_utils)

add_executable(c_tests c_tests/main.c)

