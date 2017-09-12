#include "logger.h"
using string = std::string;

Logger::Logger()
{
	m_fs.open(LogFileName, std::fstream::out);
}

Logger::~Logger()
{
	m_fs.flush();
	m_fs.close();
}

void Logger::Log(LogLevel level, string text)
{
	auto prefix = LogLevelToPrefix(level);
	m_fs << prefix << text;
}

string Logger::LogLevelToPrefix(LogLevel level)const
{
	switch (level)
	{
	case Debug:
		return "[Debug]";
	case Info:
		return "[Info]";
	case Warning:
		return "[Warning]";
	case Error:
		return "[Error]";
	default:
		return "[Unknown]";
	}
	return "";
}