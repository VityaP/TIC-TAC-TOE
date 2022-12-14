cmake_minimum_required(VERSION 3.11)
project(server_zmq_tic_tac_toe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

# use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.h
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

# set(SOURCE_EXE main.cpp)
set(SOURCE_FILES functions.cpp functions.h message.h server.cpp)

add_executable(server_zmq_tic_tac_toe ${SOURCE_FILES})

## add the include directory to our compile directives
target_include_directories(server_zmq_tic_tac_toe PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(server_zmq_tic_tac_toe PUBLIC ${ZeroMQ_LIBRARY})