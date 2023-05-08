cmake_minimum_required(VERSION 3.16)
project(AnTrans-RI
    VERSION 0.1
    DESCRIPTION "An(alyzing) Trans(pose) for LibRI."
    LANGUAGES CXX
)
include_directories(${CMAKE_SOURCE_DIR}/include)

set (CMAKE_CXX_STANDARD 17)
include (CTest)
enable_testing()
find_package(GTest HINTS /usr/local/lib/ ${GTEST_DIR})
if(NOT ${GTest_FOUND})
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG "origin/main"
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(googletest)
endif()

function(AddTest) # function for UT
    cmake_parse_arguments(UT "DYN" "TARGET" "LIBS;DYN_LIBS;STATIC_LIBS;SOURCES;DEPENDS" ${ARGN})
    add_executable(${UT_TARGET} ${UT_SOURCES})
    target_link_libraries(${UT_TARGET} ${UT_LIBS} 
        GTest::gtest_main GTest::gmock_main)
    add_test(NAME ${UT_TARGET}
      COMMAND ${UT_TARGET}
      WORKING_DIRECTORY $<TARGET_FILE_DIR:${UT_TARGET}>
    )
endfunction(AddTest)

AddTest(
    TARGET opt_test
    SOURCES tests/test_opts.cpp
)

AddTest(
    TARGET deduce_test
    SOURCES tests/test_deduce.cpp
)