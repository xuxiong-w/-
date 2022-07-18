#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*引入连接Mysql的头文件*/
#include "mysql.h"
//线程相关头文件
#include<pthread.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>
//时间相关头文件
#include<time.h>
#include <unistd.h>

/*定义一些数据库连接需要的宏*/
#define HOST "localhost" /*MySql服务器地址*/
#define USERNAME "root" /*用户名*/
#define PASSWORD "123456" /*数据库连接密码*/
#define DATABASE "data_base" /*需要连接的数据库*/
#define UPDATE "update ta_ble set x = x+1,y=y+1 where id = 1002;"
#define SELECT "select * from ta_ble where id=1002;"


pthread_rwlock_t rwlock;
MYSQL my_connection;


// 执行sql语句的函数
void exeSql(char* sql);
void *takeSql(void* addr);
void update_query(void* query);
void select_query(void* query);

void main(){
    key_t key;
    key=0;
    int arg=1;

    char commond[2][100]={"update ta_ble set x = x+1,y=y+1 where id = 1002;",
                          "select * from ta_ble where id=1002;"};

    int i;
    int ret=-1;
    extern MYSQL my_connection;
    mysql_thread_init();
    mysql_init(&my_connection);
    if(mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) {
        printf("数据库连接成功！\n");
        pthread_t tid1, tid2;
        int err;
        // 初始化读写锁
        err = pthread_rwlock_init(&rwlock, NULL);
        if(!err){
            printf("init thread_rwlock failed \n");
            return ;
        }
        err = pthread_create(&tid1, NULL, update_query, UPDATE);
        if(err){
            printf("new thread 1 create failed \n");
            return ;
        }

        //创建新线程2,失败直接退出
        err = pthread_create(&tid2, NULL, select_query, SELECT);
        if(err){
            printf("new thread 2 create failed \n");
            return ;
        }

        //等待线程1,2运行完再结束
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        //销毁锁
        pthread_rwlock_destroy(&rwlock);

        /*不要忘了关闭连接*/
        mysql_close(&my_connection);
        mysql_thread_end();
    }else {
        printf("数据库连接失败！\n");
    }
}

void update_query(void* query){
    pthread_rwlock_wrlock(&rwlock);
    char s[100];
    strcpy(s,(char*)query);
    exeSql(s);
}

void select_query(void* query){

}


void exeSql(char* sql){
    extern MYSQL my_connection;
    int res;  /*执行sql语句后的返回标志*/
    MYSQL_RES* res_ptr; /*执行结果*/
    MYSQL_ROW result_row; /*按行返回查询信息*/
    int row, column; /* 定义行数,列数*/
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
    }
}