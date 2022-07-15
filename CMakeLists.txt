cmake_minimum_required(VERSION 3.18)
project(learn_7_14_ C)

set(CMAKE_C_STANDARD 11)
set(INC_DIR /usr/include/mysql/)
set(LINK_DIR /usr/lib/x86_64-linux-gnu/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(mysqlclient)
include_directories(mysql)
include_directories(mysql/mysql)


add_executable(learn_7_14_
        mysql/mysql/client_plugin.h
        mysql/mysql/plugin_auth_common.h
        mysql/mysql/udf_registration_types.h
        mysql/errmsg.h
        mysql/field_types.h
        mysql/my_command.h
        mysql/my_compress.h
        mysql/my_list.h
        mysql/mysql.c
        mysql/mysql.h
        mysql/mysql_com.h
        mysql/mysql_time.h
        mysql/mysql_version.h
        mysql/mysqld_error.h
        mysql/mysqlx_ername.h
        mysql/mysqlx_error.h
        mysql/mysqlx_version.h)

target_link_libraries(learn_7_14_  mysqlclient)