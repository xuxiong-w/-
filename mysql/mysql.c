#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*引入连接Mysql的头文件*/
#include "mysql.h"

/*定义一些数据库连接需要的宏*/
#define HOST "192.168.5.230" /*MySql服务器地址*/
#define USERNAME "root" /*用户名*/
#define PASSWORD "ljh" /*数据库连接密码*/
#define DATABASE "finance" /*需要连接的数据库*/

// 执行sql语句的函数
void exeSql(char* sql) {
    MYSQL my_connection; /*数据库连接*/ 
    int res;  /*执行sql语句后的返回标志*/ 
    MYSQL_RES* res_ptr; /*执行结果*/ 
    MYSQL_ROW result_row; /*按行返回查询信息*/ 
    int row, column; /* 定义行数,列数*/
    mysql_init(&my_connection);
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) {
        printf("数据库连接成功！");
        /*设置查询编码为 utf8, 支持中文*/
        mysql_query(&my_connection, "set names utf8");
        res = mysql_query(&my_connection, sql);      
        if (res) {
            /*现在就代表执行失败了*/
            printf("Error： mysql_query !\n");
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        } else {
            /*现在就代表执行成功了*/
            /*mysql_affected_rows会返回执行sql后影响的行数*/
            printf("%d 行受到影响！\n", mysql_affected_rows(&my_connection));
            // 把查询结果装入 res_ptr
            res_ptr = mysql_store_result(&my_connection);
            // 存在则输出
            if (res_ptr) {
                // 获取行数，列数
                row = mysql_num_rows(res_ptr);
                column = mysql_num_fields(res_ptr);
                // 执行输出结果,从第二行开始循环（第一行是字段名）
                for (int i = 1; i < row + 1; i++) {
                    // 一行数据
                    result_row = mysql_fetch_row(res_ptr);
                    for (int j = 0; j < column; j++) {
                    	printf("%s\n", result_row[j]);
                    }
                }
            }
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        }
    } else {
        printf("数据库连接失败！");
    }
}
void main(){
    char s[] = "update client set c_name = '罗俊辉' where c_id = 100; ";
    exeSql(s);
    }
