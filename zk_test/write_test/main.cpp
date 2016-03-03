/*
Date: 3 March 2016
Author: Jingyu Yang
Description: A demo to write a buffer into ZooKeeper.
*/

#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include <zookeeper/zookeeper.h>

#define ZK_CONN_CFG "127.0.0.1:2181"
#define TEST_PATH "/test"
using namespace std;

zhandle_t * handle=NULL;
const int TEST_CNT=1000;
const int test_len=512;
void do_test(zhandle_t* handle);

void gen_test(string& buff){
    int len = buff.size();
    for (int i=0;i<len;i++){
        buff[i]='a'+rand()%10;
    }
}
void create_completion(int rc, const char *name, const void * data) {
    //printf("create finished at thread %d\n",(int)pthread_self());
    struct timeval *ptr_start = (struct timeval *)data;
    struct timeval end;
    gettimeofday(&end, NULL);
    long secs_used,micros_used;
    secs_used=(end.tv_sec - ptr_start->tv_sec); 
    micros_used= ((secs_used*1000000) + end.tv_usec) - (ptr_start->tv_usec);
    if (rc) {
        printf("%d error: %s\n",rc,zerror(rc));        
    }else{
        printf("write_opt_us %ld\n",micros_used);
    }
}
void do_test(zhandle_t* handle){
    if(NULL==handle){
        printf("handle has not been initialed.\n");
        return;
    }
    struct timeval *ptr_start = (struct timeval *)malloc(sizeof(struct timeval));
    string key = string(TEST_PATH);
    key.append("/batch-");
    string test_buff = string(test_len,' ');
    gen_test(test_buff);
    //printf("key:%s;val:%s;len:%lu\n",key.c_str(),test_buff.c_str(),test_buff.size());
    gettimeofday(ptr_start, NULL);
    int rc = zoo_acreate(handle, key.c_str(), test_buff.c_str(), test_buff.size(),
               &ZOO_OPEN_ACL_UNSAFE,
               ZOO_SEQUENCE|ZOO_EPHEMERAL,
               create_completion, ptr_start); 
    if (ZOK != rc){
        printf("%d error: %s\n",rc,zerror(rc));        
    }

}

void* thread_entry(void* data){
    printf("send req at thread %d\n",(int)pthread_self());
    srand(time(NULL));
    //int loop=50;
    for (int i=0;i<TEST_CNT;i++){
        //printf("sending No.%d\n",i);
        do_test(handle);
        usleep(1000*10); //0.01s
    }
    printf("test finished\n");
    sleep(5);
    return NULL;
}
int main(){
    printf("App will continue to write a buffer into ZooKeeper\n");   
    //zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    printf("main thread:%d\n",(int)pthread_self());
    handle = zookeeper_init(ZK_CONN_CFG,NULL,1000,NULL,NULL,0);
    if (handle){
        printf("connected to %s\n",ZK_CONN_CFG);
        // create test dir /test
        int rc = zoo_create(handle,TEST_PATH,NULL,0,&ZOO_OPEN_ACL_UNSAFE,0,NULL,0);
        if (ZOK==rc || ZNODEEXISTS==rc){
            printf("%s created\n",TEST_PATH);
            pthread_t tid;
            int trc = pthread_create(&tid,NULL,thread_entry,NULL); 
            if (0==trc){
                trc=pthread_join(tid,NULL);
                if (0==trc){
                    printf("thread finished.\n");
                }else{
                    printf("join failed.\n");
                }
            }else{
                printf("thread create failed.\n");
            }
        }else{
            printf("create root failed.\n");
        }
    }    
    zookeeper_close(handle);
}
