#include<iostream>
#include "threadpool.h"
#include<stdlib.h>

class Task
{
public:
     virtual void doit(int tid) = 0;
};
class Print_Task : public Task
{
public:
    void doit(int tid)
    {
	std::cout << tid <<" ==> hello world!" << std::endl;
    }
};

int main()
{
    PthreadPool<Task> pp(5);
    pp.start();
    while(1)
    { 
        //sleep(1);
        Task* pt = new Print_Task();
        pp.push_append(pt);
        sleep(1);
        delete pt;
    }
    exit(0);
}
