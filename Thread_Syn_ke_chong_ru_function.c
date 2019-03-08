//
// Created by yanpan on 2019/3/8.
//
#if 0
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void* fun(void *arg)
{
    char arr[] = "a b c d e f g h l m";
    char *q = NULL;
    char *p = strtok_r(arr, " ", &q);
    while(p != NULL)
    {
        printf("fun:: %s\n", p);
        sleep(1);
        p = strtok_r(NULL, " ", &q);
    }
    return NULL;
}

int main()
{
    pthread_t id;
    int res = pthread_create(&id, NULL, fun, NULL);
    assert(0 == res);
    char buff[] = "1 2 3 4 5 6 7 8 9";
    char *q = NULL;
    char *p = strtok_r(buff, " ", &q);
    while(p != NULL)
    {
        printf("main:: %s\n", p);
        sleep(1);
        p = strtok_r(NULL, " ", &q);
    }
    pthread_exit(NULL);
//    return 0;
}
#endif