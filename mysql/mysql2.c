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
#include <semaphore.h>

/*定义一些数据库连接需要的宏*/
#define HOST "localhost" /*MySql服务器地址*/
#define USERNAME "root" /*用户名*/
#define PASSWORD "123456" /*数据库连接密码*/
#define DATABASE "data_base" /*需要连接的数据库*/


pthread_mutex_t mutex;
static int  line_num=2; //同时两个线程
sem_t first;
sem_t second;

// 执行sql语句的函数
void exeSql(char* sql);
void *takeSql(void* addr);

/*
void main(){
    char s[] = "update client set c_name = '罗俊辉' where c_id = 100; ";
    exeSql(s);
    }
*/
void main(){
    char commond[2][100]={"update ta_ble set x = x+1,y=y+1 where id = 1002;",
                          "select * from ta_ble where id=1002;"};

    int i;
    int ret=-1;
    pthread_t npid[line_num];
    pthread_mutex_init(&mutex, NULL);
    for(i=0;i<line_num;i++){	//for循环创建line_num个线程
        //sleep(1);
        ret=-1;
        int x=i;
        ret=pthread_create(&npid[i],NULL,takeSql,(void *)(commond[x]));
        //printf("%d\n",commond[x]);
        if(ret)
            printf("%d create fail!\n",i+1);
        else
            printf("%d create succcess!\n",i+1);
    }
    for(i=0;i<line_num;i++){		//等待线程结束
        pthread_join(npid[i],NULL);
    }


}

void *takeSql(void *addr){
    char s[100];
    strcpy(s,(char*)addr);
    //printf("%s\n",s);
    int num=0;	//每个线程最多10次操作
    while(1){
        //printf("%s\n",s);
        pthread_mutex_lock(&mutex);
        //sleep(1);
        if(num==10){	//当此时退出
            pthread_mutex_unlock(&mutex);
            break;
        }
        exeSql(s);

        printf("No.%d:  %s commond finished!\n",num,s);
        num++;
        pthread_mutex_unlock(&mutex);

    }
}



void exeSql(char* sql){
    MYSQL my_connection; /*数据库连接*/
    mysql_thread_init();
    int res;  /*执行sql语句后的返回标志*/
    MYSQL_RES* res_ptr; /*执行结果*/
    MYSQL_ROW result_row; /*按行返回查询信息*/
    int row, column; /* 定义行数,列数*/
    mysql_init(&my_connection);
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 3306, NULL, CLIENT_FOUND_ROWS)) {
        printf("数据库连接成功！\n");
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
                MYSQL_FIELD *field = mysql_fetch_fields(res_ptr);
                for(int i = 0; i < column; i++)
                {
                    printf("%-10s\t", field[i].name);
                }
                puts("");
                // 执行输出结果,从第二行开始循环（第一行是字段名）
                MYSQL_ROW line;
                for(int i = 0; i < row; i++)
                {
                    line =  mysql_fetch_row(res_ptr);
                    for(int j = 0; j < column; j++)
                    {
                        printf("%-10s\t", line[j]);
                    }
                    puts("");
                }
            }
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
            mysql_thread_end();
        }
    } else {
        printf("数据库连接失败！\n");
    }
}