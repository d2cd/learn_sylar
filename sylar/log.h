#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__
	

#include<string>
#include<memory>
#include<stdint.h>
#include<list>
#include<sstream>
#include<fstream>
#include<vector>
#include<tuple>
#include<map>
#include"singleton.h"
#include<stdarg.h>
#include"util.h"

#define SYLAR_LOG_LEVEL(logger, level)\
		if(logger->getLevel() <= level)\
			sylar::logEventWrap(std::shared_ptr<sylar::LogEvent>(new sylar::LogEvent(logger,level,__FILE__, __LINE__, 0,sylar::GetThreadId(),sylar::GetFiberId(), time(nullptr),""))).getSS()\
		
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)\
		if(logger->getLevel() <= level)\
			sylar::logEventWrap(std::shared_ptr<sylar::LogEvent>(new sylar::LogEvent(logger,level,__FILE__, __LINE__, 0,sylar::GetThreadId(),sylar::GetFiberId(), time(nullptr),""))).getEvent()->format(fmt, __VA_ARGS__)\
		
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::INFO， fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::WARN， fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::ERROR， fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::FATAL， fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::getInstance()->getRoot() //这里注意单例模式，static T* getinstance()的使用。
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::getInstance()->getLogger(name)
namespace sylar{
//日志级别
class LogLevel{
	public:
		enum Level{
			UNKNOWN=0,
			DEBUG= 1,
			INFO = 2,
			WARN = 3,
			ERROR= 4,
			FATAL= 5
		};	
		static const char* toString(LogLevel::Level level);
		static LogLevel::Level fromString(const std::string& str);

};
class Logger;
class LoggerManager;

//日志器
class LogEvent{
	public:
		typedef std::shared_ptr<LogEvent> ptr;
		LogEvent(std::shared_ptr<Logger>logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t FiberId,\
		time_t _time, std::string m_content);
		const char* getFile() const{
			return m_file;
		}
		int32_t getLine() const 	{return m_line;}
		uint32_t getElapse() const 	{return m_elapse;}
		uint32_t getThreadId() const 	{return m_threadId;}
		uint32_t getFiberId() const 	{return m_fiberId;}
		time_t getTime() const 	{return m_time;}
		std::stringstream& getSS(){return m_ss;}
		const std::string getContent()const{return m_ss.str();}
		std::shared_ptr<Logger> getLogger(){return m_logger;}
		const LogLevel::Level getLevel()const{return m_level;}

		void format(const char* fmt, ...);
		void format(const char* fmt, va_list al);

		
	private:
		const char* m_file = nullptr; //文件名
		int32_t m_line = 0;		//行号
		uint32_t m_elapse = 0; 		//程序启动到现在的毫秒数
		uint32_t m_threadId = 0;	//线程号
	    uint32_t m_fiberId = 0;		//协程号
		time_t m_time;		//时间
		std::string m_content;		
		std::stringstream m_ss;
		std::shared_ptr<Logger> m_logger;
		LogLevel::Level m_level;
};


//日志格式器
class LogFormatter{
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
		LogFormatter(const std::string& pattern);
		void init();
		const bool isError() const {
			return m_error;
		}
		class FormatItem{
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			virtual ~FormatItem(){}
			virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,  LogEvent::ptr event) = 0;
		};
		const std::string getPattern()const{
			return m_pattern;
		}
	private:
		std::string m_pattern; //日志格式
		std::vector<FormatItem::ptr> m_items;
		bool m_error = false; //日志格式是否不对
		// FormatItem::ptr m_formatter;
};
//日志输出
class LogAppender{
	public:
		typedef std::shared_ptr<LogAppender> ptr;
		virtual ~LogAppender(){}
		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
	    void setFormatter(LogFormatter::ptr val){m_formatter = val;}
		LogFormatter::ptr getFormatter() const {return m_formatter;}
		void setLevel(LogLevel::Level level){m_level = level;}
		virtual std::string toYamlString() = 0; //这里必须写=0表明纯虚函数
	protected:
		LogLevel::Level m_level;
		LogFormatter::ptr m_formatter;
};
//日志器
class Logger : public std::enable_shared_from_this<Logger>{
	friend class LoggerManager;
	public:
		typedef std::shared_ptr<Logger> ptr;
		Logger(const std::string& name = "root");
		
		void log(LogLevel::Level level, LogEvent::ptr event);
		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);
		
		void addAppender(LogAppender::ptr appender);
	       	void delAppender(LogAppender::ptr appender);
		LogLevel::Level getLevel() const {return m_level;}
		void setLevel(LogLevel::Level val){m_level = val;}
		//获得所有的appender
		std::list<LogAppender::ptr> getAppenders(){
			return m_appenders;
		}
		void setAppenders(const std::list<LogAppender::ptr>& appenders){
			m_appenders = appenders;
		}
		void clearAppenders(){
			m_appenders.clear();
		}

		const std::string getName() const {return m_name;}
		void setFormatter(const LogFormatter::ptr val);
		void setFormatter(const std::string& val);
		LogFormatter::ptr getFormatter();
		//为什么不在config.h中设置偏特化，
		//我认为原因是这个是日志系统，而之前是配置系统。
		//功能上可以再加一个logger的偏特化可以达到相同的效果
		std::string toYamlString();
	private:
		std::string m_name;			//日志名称
		LogLevel::Level m_level;		//日志级别
		std::list<LogAppender::ptr> m_appenders; 	//日志Appender集合
		LogFormatter::ptr m_formatter;
		Logger::ptr m_root;

};
//输出到控制台
class StdoutLogAppender : public LogAppender{
	public:
	// StdoutLogAppender(const std::string fmt = ""){}
	std::string toYamlString()override;
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};
//输出到文件
class FileLogAppender : public LogAppender{
	public:
	std::string toYamlString()override;
	std::shared_ptr<FileLogAppender> ptr;
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
	FileLogAppender(const std::string& filename);
	void reopen();
	private:
	std::string m_filename;
	std::ofstream m_filestream;
	
};

class logEventWrap{
public:
	logEventWrap(LogEvent::ptr event);
	~logEventWrap();
	std::stringstream& getSS();
	LogEvent::ptr getEvent();
private:
	LogEvent::ptr m_event;
};

class LoggerManager{
public:	
LoggerManager();
void addLogger(Logger::ptr logger, std::string loggerName);
Logger::ptr getLogger(std::string logName);
Logger::ptr getRoot(){return m_root;}
void init();
std::string toYamlString();

private:
Logger::ptr m_root;
std::map<std::string, Logger::ptr> m_logger;

};


//单例模式管理，确保只有一个LoggerManager
typedef sylar::Singleton<LoggerManager> LoggerMgr;




}
#endif // __SYLAR_LOG_H__
