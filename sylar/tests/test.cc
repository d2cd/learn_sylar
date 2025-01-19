#include <iostream>
#include <thread>
#include "log.h"
#include "util.h"
int main(int argc, char** argv){
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    sylar::LogEvent::ptr event(new sylar::LogEvent(logger,sylar::LogLevel::Level::DEBUG,__FILE__, __LINE__, 0,sylar::GetThreadId(),sylar::GetFiberId(), time(nullptr),""));
    logger->log(sylar::LogLevel::DEBUG, event);
    

    auto l = sylar::LoggerMgr::getInstance()->getLogger("xx");
    SYLAR_LOG_INFO(l) << "INFO";

    sylar::LogAppender::ptr fileApp(new sylar::FileLogAppender("./log.txt"));
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%m%n"));
    fileApp->setFormatter(fmt);
    logger->addAppender(fileApp);

    SYLAR_LOG_WARN(logger) << "hello";
    SYLAR_LOG_FATAL(logger) << "hello";
    SYLAR_LOG_FMT_DEBUG(logger, "test macro fmt error %s", "-->ok fine"); 


    // std::cout << sylar::GetThreadId()   << std::endl;
    // std::cout << std::this_thread::get_id()   << std::endl;

    return 0;
}