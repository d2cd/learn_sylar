#include"log.h"



namespace sylar{
Logger::Logger(const std::string& name):m_name(name){

}

void Logger::addAppender(LogAppender::ptr appender){

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
		for(auto& it : m_appenders){
			it->log(level, event);
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


bool FileLogAppender::reopen(){
	if(m_filestream){
		m_filestream.close();
	}
	m_filestream.open(m_filename);

}
FileLogAppender::FileLogAppender(const string& filename):m_filename(filename){

}

FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
	if(level >= m_level){
		m_filestream << m_formatter.format(event);
	}
}

StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
	if(level >= m_level){
		std::cout << m_formatter.format(event);
	}
}

LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){

}

std::string LogFormatter::format(LogEvent::ptr event){
	std::stringstream ss;
	for(auto& i : m_items){
		i->format(ss, event);
	}
	return ss.str();

}

LogFormatter::init(){
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
			if(isspace(m_pattern[n])) {
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
		}

		if(fmt_status == 0){
			if(!nstr.empty()){
				vec.push_back(std::make_tuple(nstr, "", 0));
			}

			str = m_pattern.substr(i+1, n-i-1);
			vec.push_back(std::make_tuple(str, fmt, 1));

			i=n;
		}else if(fmt_status == 1){
			std::cout << "pattern phase error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
		       	vec.push_back(std::make_tuple("<<pattern error>>", fmt, 0));	
		}else if(fmt_status == 2){
			if(!nstr.empty()){
				vec.push_back(std::make_tuple(nstr, "", 0));
			}
			vec.push_back(std::make_tuple(str, fmt, 1));
			i=n;
		}
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


}


}
