#include "log.h"
#include "config.h"
#include "util.h"
#include "thread.h"

#include<vector>
sylar::Logger::ptr log_root = SYLAR_LOG_ROOT();
void fun(){
    SYLAR_LOG_INFO(log_root) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadId()
                             << " this.id " << sylar::Thread::GetThis()->getId();
    // sleep(50);
    
}

int main(){
    std::vector<sylar::Thread::ptr> vc;
    for(int i=0; i<5; i++){
        vc.push_back(sylar::Thread::ptr(new sylar::Thread(&fun, "name " + std::to_string(i))));
    }
    for(int i=0; i<5; i++){
        vc[i]->join();
    }
    SYLAR_LOG_INFO(log_root) << "thread test end";
    return 0;
}
//问题：name和this.name有什么区别
// makefile如何改ss