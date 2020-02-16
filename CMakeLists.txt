cmake_minimum_required(VERSION 3.15)
project(qhycfw3_usb)

set(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE X2_INTERFACES liscensedinterfaces/*.h)
FILE(GLOB HEADERS src/*.h)
FILE(GLOB SOURCES src/*.cpp)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fvisibility=hidden)

add_library(qhycfw3_usb SHARED ${X2_INTERFACES} ${SOURCES} ${HEADERS})
include_directories(src ${PROJECT_SOURCE_DIR})
link_libraries(unwind)
