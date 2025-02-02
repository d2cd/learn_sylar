#include "log.h"
#include <stdint.h>
#include <ctime>
#include <map>
#include <functional>
#include <iostream>
#include "config.h"


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
	if(name == "root"){
		m_appenders.push_back(LogAppender::ptr(new StdoutLogAppender));
	}
}
std::string Logger::toYamlString(){
	YAML::Node node;
	node["name"] = m_name;
	node["level"] = LogLevel::toString(m_level);
	if(m_formatter)
		node["formatter"] = m_formatter->getPattern();
	if(!m_appenders.empty()){
		for(auto& ap: m_appenders){
			node["appenders"].push_back(YAML::Load(ap->toYamlString()));
		}
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}
void Logger::setFormatter(const LogFormatter::ptr val){
	if(val->isError()) return;
	m_formatter = val;
}
void Logger::setFormatter(const std::string& val){
	auto new_formatter = LogFormatter::ptr(new LogFormatter(val));
	if(!new_formatter->isError()){
		m_formatter = new_formatter;
	}
	else{
		std::cout << "Logger set Formatter Error. name:" << m_name
				  << " Value" << val << " invalid formatter."; 
	}
}
LogFormatter::ptr Logger::getFormatter(){
	return m_formatter;
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

LogLevel::Level LogLevel::fromString(const std::string& level){
	auto t_level = level;
	std::transform(t_level.begin(), t_level.end(), t_level.begin(), ::toupper);
	
	#define XX(name)\
	if(t_level == #name)\
		return LogLevel::name;\

	XX(DEBUG);
	XX(INFO);
	XX(WARN);
	XX(ERROR);
	XX(FATAL);
	#undef XX
	return LogLevel::UNKNOWN;
		
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
			os << event->getLogger()->getName();	
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
	if(level >= m_level){
		if(!m_appenders.empty()){
			auto self = shared_from_this();
			for(auto& it : m_appenders){
				it->log(self, level, event);
			}	
		}else if(m_root){
			m_root->log(level, event);
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
std::string FileLogAppender::toYamlString(){
	YAML::Node node;
	node["type"] = "FileLogAppender";
	node["file"] = m_filename;
	std::stringstream ss;
	ss << node;
	return ss.str();
}
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

std::string StdoutLogAppender::toYamlString(){
	YAML::Node node;
	node["type"] = "StdoutLogAppender";
	node["level"] = LogLevel::toString(m_level);
	node["formatter"] = m_formatter->getPattern();
	std::stringstream ss;
	ss << node;
	return ss.str();
}
LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){
	m_pattern = pattern;
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
			m_error = true;
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
				m_error = true;
			}
			else{
				m_items.push_back(FormatItem::ptr(it->second(std::get<0>(i))));
			}
		}

		// std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) <<std::endl;
	
	}
}

LoggerManager::LoggerManager(){
	m_root.reset(new Logger("m_root")); //这里一定不能是"root"，因为config在lookup时候也会new一个"root"
	m_root->addAppender(LogAppender::ptr(new sylar::StdoutLogAppender));
	init();
}
std::string LoggerManager::toYamlString(){
	YAML::Node node;
	if(m_logger.empty()){
		return m_root->toYamlString();
	}
	for(auto& m_log : m_logger){
		node[m_log.first] = YAML::Load(m_log.second->toYamlString());
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
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
	if(it != m_logger.end()){
		return m_logger[loggerName];
	}
	auto new_logger = Logger::ptr(new Logger(loggerName));
	// new_logger->setAppenders(m_root->getAppenders());
	new_logger->m_root = m_root; //loggerManager 是 logger的友元, 这里new_logger其实还是没有appender所以可以使用root中的appender写
	m_logger[loggerName] = new_logger;
	return new_logger;
}
struct LogAppenderDefine{
	uint32_t type = 0; // 1 File 2 Std
	LogLevel::Level level = LogLevel::UNKNOWN;
	std::string formatter;
	std::string file;
	bool operator==(const LogAppenderDefine& v) const{
		return type == v.type &&
				level == v.level &&
				formatter == v.formatter &&
				file == v.file;
	}
};

struct LogDefine{
	std::string name;
	LogLevel::Level level = LogLevel::UNKNOWN;
	std::string formatter;
	std::vector<LogAppenderDefine> appenders;
	
	bool operator==(const LogDefine& v) const{
		return name == v.name &&
			   level == v.level &&
			   formatter == v.formatter &&
			   appenders == v.appenders;
	}
	bool operator<(const LogDefine& v)const{
		return name < v.name;
	}
};


template<>
class LexicalCast<std::set<LogDefine>, std::string> {
public:
    std::stringstream ss;
    std::string operator()(std::set<LogDefine>& st){
		YAML::Node node;
        for(auto v : st)
        {
			node["name"] = v.name;
			node["level"] = LogLevel::toString(v.level);
			node["formatter"] = v.formatter;
			for(size_t i=0; i<v.appenders.size(); i++){
				YAML::Node nn;
				auto t = v.appenders[i];
				if(t.type == 1){
					nn["type"] = "FileLogAppender";
				}else if(t.type == 2){
					nn["type"] = "StdoutLogAppender";
				}
				nn["formatter"] = t.formatter;
				nn["level"] = LogLevel::toString(t.level);
				node["appender"].push_back(nn);
			}
        }
        
        ss << node;
        return ss.str();
    }  
};
template<>
class LexicalCast<std::string, std::set<LogDefine>>{
public:
    std::set<LogDefine> operator()(std::string v){
        std::stringstream ss;
		ss.str("");
        YAML::Node node = YAML::Load(v); //是以字典的形式保存, k和v
        std::set<LogDefine> ret;
        for(size_t i=0; i<node.size(); i++){
			auto n = node[i];
			if(!n["name"].IsDefined()){
				std::cout << "Log config error: name is null.  " << n << std::endl; 
				continue;
			}
			LogDefine ld;
			ld.name = n["name"].as<std::string>(); 
			ld.level = LogLevel::fromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
			if(n["formatter"].IsDefined()){
				ld.formatter = n["formatter"].as<std::string>();
			}
			if(n["appender"].IsDefined()){
				for(size_t x=0; x<n["appender"].size(); ++x){
					auto a = n["appender"][x];
					LogAppenderDefine ad; //type level formatter file
					if(a["type"].IsDefined()){
						auto type_str = a["type"].as<std::string>();
						if(type_str == "FileLogAppender"){
							ad.type = 1;
							if(!a["file"].IsDefined()){
								std::cout << "Log config error: Fileappender file is null.  " << a << std::endl; 
								continue;
							}
							ad.file = a["file"].as<std::string>();
						}else if(type_str == "StdoutLogAppender"){
							ad.type = 2;
						}else{
							ad.type = 0;
						}
					}else{
						std::cout << "Log config error: appender type is null.  " << a << std::endl; 
						continue;
					}
					//如果appender没有设置level那么就是用logger的level
					ad.level = LogLevel::fromString(a["level"].IsDefined() ? a["level"].as<std::string>() : n["level"].as<std::string>());
					ad.formatter = a["formatter"].IsDefined() ? a["formatter"].as<std::string>() : n["formatter"].as<std::string>();
					ld.appenders.push_back(ad);
				}
			}
			ret.insert(ld);
		}
        return ret;
    } 	
};

//logs包含了所有的log, g_log_defines是一个包含了所有log配置的manager
// 执行代码后，config里面会产生一个名为“logs”的configVar
sylar::ConfigVar<std::set<LogDefine>>::ptr g_log_defines = 
	sylar::Config::Lookup("logs", std::set<LogDefine>(), "log config");


struct LogIniter{
	LogIniter()
	{//为什么这里没有执行呢
		std::cout << "hello in LogIniter!!!" << std::endl;
		g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine>& old_val, 
								const std::set<LogDefine>& new_val)
								{
									SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
									//新增 新的里面有，老的里面没有
									for(auto& i : new_val){
										Logger::ptr new_logger;
										auto it = old_val.find(i);
										if(it == old_val.end()){
											//增加一个logger
											new_logger = SYLAR_LOG_NAME(i.name);
										}
										else
										{
											//修改 都有 看看一不一样
											if(!(i == *it)){
												new_logger = SYLAR_LOG_NAME(i.name);
											}else{
												continue;
											}
										}
										new_logger->setLevel(i.level);
										//new_logger本身包含正确的formatterr
										//如果解析处理错的，那就没必要改logger的formatter,本身有正确的formatter
										if(!i.formatter.empty())
											new_logger->setFormatter(i.formatter);
										
										new_logger->clearAppenders();//清空
										for(auto& t : i.appenders){
											LogAppender::ptr ap;
											if(t.type == 1){
												ap.reset(new FileLogAppender(t.file));
												ap->setLevel(t.level);
												new_logger->addAppender(ap);
											}else if(t.type == 2){
												ap.reset(new StdoutLogAppender);
												ap->setLevel(t.level);
												new_logger->addAppender(ap);
											}
			
										}
									
										//删除 老的有 新的没有
										for(auto& i : old_val){
											auto it = new_val.find(i);
											if(it == new_val.end()){
												//我们不能真删除，可以把日志级别设置的很高
												auto logger_t = SYLAR_LOG_NAME(i.name);
												logger_t->setLevel((LogLevel::Level)100);
												logger_t->clearAppenders();
											}
										}
									
									}
								});
	}
};

//在main函数之前构造
static LogIniter __log_init;

void LoggerManager::init(){
	
}



}
