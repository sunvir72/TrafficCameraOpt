cmake_minimum_required(VERSION 2.8.12)
project(ece650-a3)

## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/run)

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# link_directories(${CMAKE_CURRENT_SOURCE_DIR})
# add_library(a3FlagsDefValsh  a3FlagsDefVals.h)

# create the main executable
## add additional .cpp files if needed
add_executable(ece650-a2 ece650-a2.cpp )
add_executable(ece650-a3 ece650-a3.cpp )
add_executable(rgen rgen.cpp )
# add_executable(a3FlagsDefVals a3FlagsDefVals.h )
add_library(a3FlagsDefVals STATIC a3FlagsDefVals.cpp )
add_library(signal_handler STATIC signal_handler.cpp )

target_link_libraries(rgen PRIVATE a3FlagsDefVals)
target_link_libraries(ece650-a3 PRIVATE signal_handler)

# create the executable for tests
add_executable(tests test.cpp)
install(PROGRAMS ece650-a1.py cityStructure.py regExHelper.py DESTINATION bin)
# install(PROGRAMS cityStructure.py DESTINATION bin)
# install(PROGRAMS regExHelper.py DESTINATION bin)
install(TARGETS ece650-a2 ece650-a3 rgen RUNTIME DESTINATION bin)
