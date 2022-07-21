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
#include<sys/time.h>
#include <unistd.h>
#include <semaphore.h>

/*定义一些数据库连接需要的宏*/
#define HOST "124.222.112.2" /*MySql服务器地址*/
#define USERNAME "root" /*用户名*/
#define PASSWORD "123456" /*数据库连接密码*/
#define DATABASE "data_base" /*需要连接的数据库*/

MYSQL my_connection;
static int  line_num=2; //同时两个线程
//pthread_mutex_t mutex;
sem_t first;
sem_t second;
struct timeval t1,t2,t3,t4;

// 执行sql语句的函数
void exeSql(char* sql);
void *updateSql(void* addr);
void *selectSql(void* addr);

/*
void main(){
    char s[] = "update client set c_name = '罗俊辉' where c_id = 100; ";
    exeSql(s);
    }
*/
int main(){
    //pthread_mutex_init(&mutex, NULL);
    sem_init(&first,0,1);
    sem_init(&second,0,0);
    char command[2][100]={"update ta_ble set x = x+1,y=y+1 where id = 1002;",
                          "select * from ta_ble where id=1002;"};

    int i;
    int ret=-1;
    pthread_t npid[line_num];
    char local_infile[] = "set global local_infile=1;";
    char load_xml[] = "LOAD XML LOCAL INFILE '/home/xxu/mysql/mysql1.xml' INTO TABLE ta_ble;";
    char truncate_table[] = "TRUNCATE ta_ble;";
    mysql_thread_init();
    mysql_init(&my_connection);
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE,
                           3306, NULL, CLIENT_LOCAL_FILES)) {
        printf("数据库连接成功！\n");
        exeSql(truncate_table);
        exeSql(local_infile);
        exeSql(load_xml);
        mysql_query(&my_connection, "set names utf8");
        ret=pthread_create(&npid[0],NULL,updateSql,(void *)(command[0]));
        if(ret)
            printf("1 create fail!\n");
        else
            printf("1 create success!\n");
        ret=pthread_create(&npid[1],NULL,selectSql,(void *)(command[1]));
        if(ret)
            printf("2 create fail!\n");
        else
            printf("2 create success!\n");
        for(i=0;i<line_num;i++){		//等待线程结束
            pthread_join(npid[i],NULL);
        }
    }
    else {
        printf("数据库连接失败！\n");
    }
    mysql_close(&my_connection);
    mysql_thread_end();
}

void *updateSql(void *addr){
    char *s;
    s = (char*) addr;
    //printf("%s\n",s);
    int num=0;	//每个线程最多10次操作
    while(1){
        sem_wait(&first);
        //pthread_mutex_lock(&mutex);
        //sleep(1);//usleep(500)休眠0.5ms
        if(num==10){	//当此时退出
            //pthread_mutex_unlock(&mutex);
            //exit(0);
            sem_post(&second);
            break;
        }
        gettimeofday(&t1,NULL);
        exeSql(s);
        gettimeofday(&t2,NULL);
        printf("%ld\n",(t2.tv_sec-t1.tv_sec)*1000000 + t2.tv_usec-t1.tv_usec);
        printf("No.%d:  %s command finished!\n",num,s);
        num++;
        //pthread_mutex_unlock(&mutex);
        sem_post(&second);
    }
}

void *selectSql(void *addr){
    char *s;
    s = (char*) addr;
    //printf("%s\n",s);
    int num=0;	//每个线程最多10次操作
    while(1){
        sem_wait(&second);
        //pthread_mutex_lock(&mutex);
        //sleep(1);//usleep(500)休眠0.5ms
        if(num==10){	//当此时退出
            //pthread_mutex_unlock(&mutex);
            //exit(0);
            sem_post(&first);
            break;
        }
        gettimeofday(&t3,NULL);
        exeSql(s);
        gettimeofday(&t4,NULL);
        printf("%ld\n",(t4.tv_sec-t3.tv_sec)*1000000 + t4.tv_usec-t3.tv_usec);
        printf("No.%d:  %s command finished!\n",num,s);
        num++;
        //pthread_mutex_unlock(&mutex);
        sem_post(&first);
    }
}

void exeSql(char* sql){
    int res;  /*执行sql语句后的返回标志*/
    MYSQL_RES* res_ptr; /*执行结果*/
    MYSQL_ROW result_row; /*按行返回查询信息*/
    unsigned long row, column; /* 定义行数,列数*/
    res = mysql_query(&my_connection, sql);
    if (res) {
        /*现在就代表执行失败了*/
        printf("Error： mysql_query !\n");
        /*不要忘了关闭连接*/
        mysql_close(&my_connection);
    } else {
        /*现在就代表执行成功了*/
        /*mysql_affected_rows会返回执行sql后影响的行数*/
        //printf("%d 行受到影响！\n", mysql_affected_rows(&my_connection));
        // 把查询结果装入 res_ptr
        res_ptr = mysql_store_result(&my_connection);
        // 存在则输出
        if (res_ptr) {
            // 获取行数，列数
            row = mysql_num_rows(res_ptr);
            column = mysql_num_fields(res_ptr);
            MYSQL_FIELD *field = mysql_fetch_fields(res_ptr);
            for(unsigned long i = 0; i < column; i++)
            {
                printf("%-10s\t", field[i].name);
            }
            puts("");
            // 执行输出结果,从第二行开始循环（第一行是字段名）
            MYSQL_ROW line;
            for(unsigned long i = 0; i < row; i++)
            {
                line =  mysql_fetch_row(res_ptr);
                for(unsigned long j = 0; j < column; j++)
                {
                    printf("%-10s\t", line[j]);
                }
                puts("");
            }
        }
    }
}