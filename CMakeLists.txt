cmake_minimum_required(VERSION 3.18)
project(learn_7_14_ C)

set(CMAKE_C_STANDARD 11)
set(INC_DIR /usr/include/mysql/)
set(LINK_DIR /usr/lib/x86_64-linux-gnu/)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(mysqlclient)
include_directories(mysql)
include_directories(mysql/mysql)

add_executable(learn_7_14_ mysql.c)

target_link_libraries(learn_7_14_  mysqlclient)
#find_package(Threads REQUIRED)
#target_link_libraries(learn_7_14_ Threads::Threads)
#add_executable(learn_7_14_ mysql.c)