# set minimum cmake version
cmake_minimum_required(VERSION 3.14)

# project name and language
project(testFileIO LANGUAGES CXX)

# enable FetchContent
include(FetchContent)

# declare Catch2
FetchContent_Declare(tinyperfstats                                    
  GIT_REPOSITORY https://github.com/HFTrader/tiny-cpp-perf-stats.git  # the repository
  GIT_TAG        fd8b8e599c5d5fa451693399995c0abea69fba73             # the tag
  )


add_executable( testFileio fileio/bench.cpp )
target_link_libraries( testFileio ${REQUIRED_LIBS} tinyperfstats)

#list( APPEND TARGETS testFileio )

# make available
FetchContent_MakeAvailable(tinyperfstats)