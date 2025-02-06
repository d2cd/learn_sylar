#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__
//C++11之前使用pthread_xxx
//c++11之后使用std::thread 其实也是pthread做的，但是没有分读写lock，因此不适合读多写少
//因此线程用std::thread， 互斥量用pthread的线程库
#include<thread>
#include<functional>
#include<memory>
#include<string>

namespace sylar{
class Thread{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void ()> cb, const std::string& name);
    ~Thread();

    void join();

    pid_t getId();
    const std::string& getName(){return m_name;}
    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
    
    static void* run(void* arg);
    
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::string m_name;
    std::function<void()> m_cb;
    Thread (const Thread& ) = delete;
    Thread (const Thread&&) = delete;
    Thread& operator=(const Thread& ) = delete;

};

}


#endif