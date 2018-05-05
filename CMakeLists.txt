cmake_minimum_required (VERSION 3.0)
project (BroadcastServer)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BINARY_DIR bin)
set (CMAKE_SOURCE_DIR src)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (EXTRA_LIBS ${EXTRA_LIBS} pthread)

add_library(Server ${CMAKE_SOURCE_DIR}/Server/Server.cpp)
add_library(Client ${CMAKE_SOURCE_DIR}/Client/Client.cpp)

include_directories("${CMAKE_SOURCE_DIR}/Server")
include_directories("${CMAKE_SOURCE_DIR}/Client")

add_executable(RunClient ${CMAKE_SOURCE_DIR}/RunClient.cpp)
target_link_libraries(RunClient pthread)
add_executable(RunServer ${CMAKE_SOURCE_DIR}/RunServer.cpp)
target_link_libraries(RunServer pthread)

