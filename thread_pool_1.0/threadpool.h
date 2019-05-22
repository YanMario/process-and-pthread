#include<iostream>
#include<stdlib.h>
#include<queue>
#include<vector>
#include"lock.h"
#include<algorithm>
#include<pthread.h>

template<typename Task>
class PthreadPool
{
public:
      PthreadPool(int num);
      ~PthreadPool();
     
     static void* worker(void* arg); 
     void start();                    
     void run();                  
     void stop();
     void push_append(Task* ps);         //添加任务
     Task* getTask();                    //得到任务
     
     void mv_to_busy(pthread_t tid);     //添加线程到忙队列中，从空闲队列剔除该线程
     void mv_to_leisure(pthread_t tid);  //添加线程到空闲队列中，从忙队列剔除该线程
     
private:
    int threadnum;                         //线程个数
    int leisurenum;                        //
    std::vector<pthread_t> busy_tid;      //忙线程队列
    std::vector<pthread_t> leisure_tid;   //空闲线程队列
    std::queue<Task*> work_que;           //任务队列
    bool is_stop;                         //是否停止
    Mutex mutex;                          //互斥锁
    Cond cond;                            //条件变量
};
template<typename T>
PthreadPool<T>::PthreadPool(int num):
	threadnum(num),leisurenum(num),is_stop(false)
	{}
template<typename T>
PthreadPool<T>::~PthreadPool()
{
    stop();
}
template<typename T>
void* PthreadPool<T>::worker(void* arg)
{
    PthreadPool<T>* ptp = (PthreadPool<T>*)arg;        //arg参数传递的是this指针
    ptp->run();                                        //调用run函数，run函数是执行任务的函数
    return ptp;                                        //this指针
}
template<typename T>     
void PthreadPool<T>::start()
{
     pthread_t tid = 0;
     for(int i =0; i < threadnum; i++)
     {
	    if(pthread_create(&tid,NULL,worker,this) != 0)   //创建线程。   worker为回调函数，执行线程的函数
        {
             std::cout << "pthread create fail!" << std::endl;
             exit(0);
        }
        leisure_tid.push_back(tid);                      //加入空闲线程队列
     }

}
template<typename T>
void PthreadPool<T>::run()
{
    pthread_t tid = pthread_self();    //得到线程的pid
    while(!is_stop)          //stop is false
    {
         T* ps = getTask();          //获取任务
         if(ps == NULL)              //判断是否为NULL
         {
            // std::cout << "no task to do!" << std::endl;
            cond.wait();             //等待条件变量成立，在此期间释放互斥锁
         }
         else
     	 {
            mv_to_busy(tid);         //将该线程加入到忙队列(通过线程的pid)
            ps->doit(tid);           //执行任务
            mv_to_leisure(tid);      //执行完任务 将该线程加入到空闲队列(通过线程的pid)
         }    

    }
}
template<typename T>
void PthreadPool<T>::stop()
{
    is_stop = true; 
    cond.broadcast();     //销毁条件变量
}
template<typename T>
void PthreadPool<T>::push_append(T* ps)             //添加任务
{
    mutex.mutex_lock();
    int leisuresize = leisurenum;                //空闲线程的个数
    work_que.push(ps);                           //将任务ps添加到任务队列
    mutex.mutex_unlock();                        
    if(leisuresize)
	cond.signal();                               //如果有空闲线程，即告诉空闲线程条件成立
}
template<typename T>
T* PthreadPool<T>::getTask()
{
   mutex.mutex_lock();
   T* ps = NULL;
   if(!work_que.empty())
   {
      ps = work_que.front();              //得到任务
      work_que.pop();                     //把任务从队列中移除
   }
   mutex.mutex_unlock();
   return ps;                             //返回任务
}
template<typename T>
void PthreadPool<T>::mv_to_busy(pthread_t tid)
{
    mutex.mutex_lock();
    std::vector<pthread_t>::iterator fit = std::find(leisure_tid.begin(),leisure_tid.end(),tid);    //找到线程的pid
    if(fit != leisure_tid.end())
    {
        leisure_tid.erase(fit);              //将该线程从空闲队列中剔除
        leisurenum--;                        //空闲线程数目减1
        busy_tid.push_back(tid);             //将线程加入到忙队列中
    }
    mutex.mutex_unlock();
}
template<typename T>
void PthreadPool<T>::mv_to_leisure(pthread_t tid)
{
    mutex.mutex_lock();
    std::vector<pthread_t>::iterator fit = std::find(busy_tid.begin(),busy_tid.end(),tid);
    if(fit != busy_tid.end())
    {
        busy_tid.erase(fit);
        leisurenum++;
        leisure_tid.push_back(tid);
    }
    mutex.mutex_unlock();
}
















