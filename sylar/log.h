#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__
	

#include<string>
#include<memory>
#include<stdint.h>
#include<list>
#include<stringstream>
#include<fstream>
#include<vector>
#include<tuple>


namespace sylar{
//日志器

class LogEvent{
	public:
		typedef std::shared_ptr<LogEvent> ptr;
		LogEvent();
	private:
		const char* m_file = nullptr; //文件名
		int32_t m_line = 0;		//行号
		uint32_t m_elapse = 0; 		//程序启动到现在的毫秒数
		uint32_t m_threadId = 0;	//线程号
	       	uint32_t m_fiberId = 0;		//协程号
		uint64_t m_time;		//时间
		std:string m_content;		
}

//日志级别
class LogLevel{

	public:
		enum Level{
			DEBUG= 1,
			INFO = 2,
			WARN = 3,
			ERROR= 4,
			FATAL= 5
		};


};
//日志格式器
class LogFormatter{
	public:
		typedef shared_ptr<LogFormatter> ptr;
		std::string format(LogEvent::ptr event);
		LogFormatter(const std::string& pattern);
		void init();
	private:
		class FormatItem{
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			virtual ~FormatItem(){}
			virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
			
		}
	private:
		std::string m_pattern; //日志格式
		std::vector<FormatItem::ptr> m_items;
};
//日志输出
class LogAppender{
	public:
		typedef shared_ptr<LogAppender> ptr;
		virtual ~LogAppender(){}
		virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;
	       	void setFormatter(LogFormatter::ptr val){m_formatter = val;}
		LogFormatter::ptr getFormatter() const {return m_formatter;}
	protected:
		LogLevel::Level m_level;
		LogFormatter::ptr m_formatter;

};
//日志器
class Logger{
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

	private:
		std::string m_name;			//日志名称
		LogLevel::Level m_level;		//日志级别
		list<LogAppender::ptr> m_appenders; 	//日志Appender集合

};
//输出到控制台
class StdoutLogAppender : public LogAppender{
	public:

	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(LogLevel::Level level, LogEvent::ptr event) override;
};
//输出到文件
class FileLogAppender : public LogAppender{
	public:
	std::shared_ptr<FileLogAppender> ptr;
	void log(LogLevel::Level level, LogEvent::ptr event) override;
	FileLogAppender(const std::string& filename);
	void reopen();
	private:
	std::string m_filename;
	std::ofstream m_filestream;
	
};



}
