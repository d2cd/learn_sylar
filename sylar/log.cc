#include "log.h"
#include <stdint.h>
#include <ctime>
#include <map>
#include <functional>
#include <iostream>


namespace sylar{
logEventWrap::logEventWrap(std::shared_ptr<LogEvent> event)\
	:m_event(event){}
logEventWrap::~logEventWrap(){
	if(m_event){
		m_event->getLogger()->log(m_event->getLevel(), m_event);
	}
}
std::stringstream& logEventWrap::getSS(){
	return m_event->getSS();
}
LogEvent::ptr logEventWrap::getEvent(){
	return m_event;
}

Logger::Logger(const std::string& name):m_name(name){
	m_level = LogLevel::Level::UNKNOWN;
	m_formatter.reset(new LogFormatter("%d%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

const char* LogLevel::toString(LogLevel::Level level){
	switch(level){
	#define XX(name) \
	case LogLevel::name: \
		return #name;\
		break;
	
		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);
	#undef XX
	default:
		return "UNKNOWN";
		break;	
	return "UNKNOWN";	
	
	} 
}



class MessageFormatItem : public LogFormatter::FormatItem{

	public:
		MessageFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getContent();	
		}
};
class LevelFormatItem : public LogFormatter::FormatItem{
	public:
		LevelFormatItem(const std::string& fmt=""){}

		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << LogLevel::toString(level);	
		}
};
class ElapseFormatItem : public LogFormatter::FormatItem{
	public:
		ElapseFormatItem(const std::string& fmt=""){}
			
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getElapse();	
		}
};
class LoggerNameFormatItem : public LogFormatter::FormatItem{
	public:
		LoggerNameFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << logger->getName();	
		}
};
class ThreadIdFormatItem : public LogFormatter::FormatItem{
	public:
		ThreadIdFormatItem(const std::string& fmt=""){}

		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getThreadId();	
		}
};
class FiberIdFormatItem : public LogFormatter::FormatItem{
	public:
		FiberIdFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getFiberId();	
		}
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
	DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : m_format(format) {
		
		m_format = "%Y-%m-%d %H:%M:%S";	//我不明白为什么这里会有问题
		// if(m_format.size() == 0) std::cout << "format error"<<std::endl;
		// else std::cout << m_format;
		// std::cout << "building dateTimeitem" << std::endl;
		// std::cout << format << std::endl;
		// std::string m;
		// std::cout << m << std::endl;
		
	}
	void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		struct tm tm;
		time_t raw_time = event->getTime();
		localtime_r(&raw_time, &tm);
		char buf[128]; // 确保这个大小足够容纳格式化后的字符串
		strftime(buf, sizeof(buf), m_format.c_str(), &tm);
		os << buf; // 将格式化后的字符串写入到os流
	}
private:
	std::string m_format;
};
class FilenameFormatItem : public LogFormatter::FormatItem{
	public:
		FilenameFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getFile();	
		}
};
class LineFormatItem : public LogFormatter::FormatItem{
	public:
		LineFormatItem(const std::string& fmt=""){}

		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << event->getLine();	
		}
};
class NewLineFormatItem : public LogFormatter::FormatItem{
	public:
		NewLineFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << std::endl;	
		}
};
class TabFormatItem : public LogFormatter::FormatItem{
	public:
		TabFormatItem(const std::string& fmt=""){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << "\t";	
		}
};
class StringFormatItem : public LogFormatter::FormatItem{
	
	public:

		StringFormatItem(const std::string& str):m_string(str){}
		void format(std::ostream &os,  Logger::ptr logger, LogLevel::Level level,  LogEvent::ptr event) override{
			os << m_string;	
		}
	private:
		std::string m_string;
};


LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,const char* file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t FiberId,\
		time_t _time, std::string content){

			m_file = file;
			m_line = line;
			m_elapse = elapse;
			m_threadId = threadId;
			m_fiberId = FiberId;
			m_time = _time;
			m_ss << content;
			m_logger = logger;
			m_level = level;
		}
void LogEvent::format(const char* fmt, ...){
	va_list al;
	va_start(al, fmt);
	format(fmt, al);
	va_end(al);
}
void LogEvent::format(const char* fmt, va_list al){
	char* buf = nullptr;
	int len = vasprintf(&buf, fmt, al);
	if(len != -1){
		m_ss << std::string(buf, len);
	}
	free(buf);
}

void Logger::addAppender(LogAppender::ptr appender){
	if(!appender->getFormatter()){
		appender->setFormatter(m_formatter);
	}
	m_appenders.push_back(appender);
 
}

void Logger::delAppender(LogAppender::ptr appender){
	for(auto it=m_appenders.begin(); it!=m_appenders.end(); ++it){
		if(*it == appender){
			m_appenders.erase(it);
			break;
		}
	}
	return ;
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
	if(level > m_level){
		auto self = shared_from_this();
		for(auto& it : m_appenders){
			it->log(self, level, event);
		}
	}
}
void Logger::debug(LogEvent::ptr event){
	log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event){
	log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event){
	log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event){
	log(LogLevel::ERROR, event);	
}
void Logger::fatal(LogEvent::ptr event){
	log(LogLevel::FATAL, event);
}


void FileLogAppender::reopen(){
	// if(m_filestream){
	// 	m_filestream.close();
	// }
	// m_filestream.open(m_filename);
	 if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);

    // 检查文件是否成功打开
    if (!m_filestream.is_open()) {
        // 处理错误，例如记录错误日志或抛出异常
        std::cerr << "Failed to open file: " << m_filename << std::endl;
        // 可以选择抛出异常
        throw std::runtime_error("Failed to open file: " + std::string(m_filename));
	}

}

FileLogAppender::FileLogAppender(const std::string& filename):m_filename(filename){m_filename =filename;}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
	reopen();
	if(level >= m_level){
		// std::cout << "in filelogappender writting" << std::endl;
		m_filestream << m_formatter->format(logger, level, event);
	}
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
	if(level >= m_level){
		std::string st = m_formatter->format(logger,level,event);
		std::cout << st;
	}
}

LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){
	
	init();

}

std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level , LogEvent::ptr event){
	// init();
	std::stringstream ss;
	for(auto& i : m_items){
		i->format(ss, logger, level, event);
	}
	return ss.str();

}

void LogFormatter::init(){
	//str pattern type
	std::vector<std::tuple<std::string, std::string, int> > vec;
	std::string nstr;
	
	for(size_t i=0; i<m_pattern.size(); i++){
		if(m_pattern[i] != '%'){
			nstr.append(1, m_pattern[i]);
			continue;
		}

		if((i+1)<m_pattern.size()){
			if(m_pattern[i+1] == '%'){
				nstr.append(1, '%');
				continue;
			}
		}

		
		size_t n = i+1;
		int fmt_status = 0;
		size_t fmt_begin = 0;
		
		std::string str;
		std::string fmt;

		while(n < m_pattern.size()){
			if(!isalpha(m_pattern[n]) && m_pattern[n]!='{' && m_pattern[n]!='}') {
				break;
			}
			
			if(fmt_status == 0){
				if(m_pattern[n] == '{'){
					str = m_pattern.substr(i+1, n-i-1);
					fmt_status = 1;
					fmt_begin =  n;
					++n;
					continue;
				}
			}
			
			if(fmt_status == 1){
				if(m_pattern[n] == '}'){
					fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
					fmt_status = 2;
					break;
				}
			}
			++n;
		}

		if(fmt_status == 0){
			if(!nstr.empty()){
				vec.push_back(std::make_tuple(nstr, "", 0));
				nstr.clear();
			}

			str = m_pattern.substr(i+1, n-i-1);
			vec.push_back(std::make_tuple(str, fmt, 1));

			i=n-1;
		}else if(fmt_status == 1){
			std::cout << "pattern phase error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
		       	vec.push_back(std::make_tuple("<<pattern error>>", fmt, 0));	
		}else if(fmt_status == 2){
			if(!nstr.empty()){
				vec.push_back(std::make_tuple(nstr, "", 0));
				nstr.clear();
			}
			vec.push_back(std::make_tuple(str, fmt, 1));
			i=n-1;
		}
		// nstr.clear();
	}
	if(!nstr.empty()){
		vec.push_back(std::make_tuple(nstr, "", 0));
	}

	//%m  消息体
	//%p  level
	//%r  启动后时间
	//%c  日志名称
	//%t  线程id
	//%n  回车换行
	//%d  时间
	//%f  文件名称
	//%l  行号
	//%T  tab
	//%F  fiberId
	
	static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
		{#str, [](const std::string& fmt){return FormatItem::ptr(new C(fmt));}}

		XX(m, MessageFormatItem),
		XX(p, LevelFormatItem),
		XX(r, ElapseFormatItem),
		XX(c, LoggerNameFormatItem),
		XX(t, ThreadIdFormatItem),
		XX(n, NewLineFormatItem),
		XX(d, DateTimeFormatItem),	
		XX(f, FilenameFormatItem),	
		XX(l, LineFormatItem),
		XX(T, TabFormatItem),
		XX(F, FiberIdFormatItem)
		//"%d [%p] %f %l %m %n"
#undef XX
	};

	for (auto& i : vec){
		if(std::get<2>(i) == 0){
			m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
		}else{
			auto it = s_format_items.find(std::get<0>(i));
			if(it == s_format_items.end()){
				m_items.push_back(FormatItem::ptr(new StringFormatItem("<< Error_format %" + std::get<1>(i) + ">>")));

			}
			else{
				m_items.push_back(FormatItem::ptr(it->second(std::get<0>(i))));
			}
		}

		// std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) <<std::endl;
	
	}
}

LoggerManager::LoggerManager(){
	m_root.reset(new Logger);
	m_root->addAppender(LogAppender::ptr(new sylar::StdoutLogAppender));
}
void LoggerManager::addLogger(Logger::ptr logger, std::string loggerName){
	if(m_logger.find(loggerName) == m_logger.end()){
		m_logger[loggerName] = logger;
	}else{
		std::cout << "logName: " << loggerName << "<EXIST>" << std::endl;
	}
	
}
Logger::ptr LoggerManager::getLogger(std::string loggerName){
	auto it = m_logger.find(loggerName);
	return (it == m_logger.end() )? m_root : m_logger[loggerName];
}



}
