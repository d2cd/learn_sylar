#include "thread.h"
#include "log.h"
#include "config.h"
#include "util.h"
namespace sylar{

static Logger::ptr log_sys = SYLAR_LOG_NAME("system");
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_name = "UNKNOWN";

Thread::Thread(std::function<void ()> cb, const std::string& name){
    m_cb.swap(cb);
    if(name.empty()){
        m_name = "UNKNOWN";
    }else{
        m_name = name;
        //这里需要注意，这里不用t_name = name
        //因为t_name是thread_local变量，此时实例化thread是在主线程中进行的
        // t_name = name 没什么用
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt){
        SYLAR_LOG_DEBUG(log_sys) << "pthread_create Error";
    }
}

Thread::~Thread(){
    if(m_thread)
        pthread_detach(m_thread);
}

pid_t Thread::getId(){
    return m_id;
}

void Thread::join(){
    if(m_thread)
    {
        int rt = pthread_join(m_thread, nullptr);
        if(rt){
            SYLAR_LOG_DEBUG(log_sys) << "pthread_join Error";
        }
        //这里m_thread必须设置为0
        m_thread = 0;
    }
}

const std::string& Thread::GetName(){
    return t_name;
}

void Thread::SetName(const std::string& name){
    
    //这里的处理很妙
    if(t_thread){
        t_thread->m_name = name;
    }
    t_name = name;
}

Thread* Thread::GetThis(){
    return t_thread;
}

void* Thread::run(void* arg){
    //此时 新的线程已经构建完成
    //改线程包含的代码文件（Thread实例）为下面thread
    Thread* thread = (Thread*) arg;

    t_thread = thread;
    t_thread->m_id = sylar::GetThreadId();
    t_name = t_thread->m_name;

    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
    // thread->m_cb();
    std::function<void()> f;
    f.swap(thread->m_cb);
    f();
    return 0;
}

}
