#pragma once
#include <stdarg.h>
#include "boost\log\trivial.hpp"

/*
부스트 로그의 trivial을 사용한 간단한 로거 구현
LoggerBase를 상속받아서 다른 방식으로 구현예정
색깔이 나왔으면 좋겠다. boost로그의 sink를 조작하면
가능할것 같은데.
*/

namespace MDUtillity
{
	const int consoleLogMaxLength = 256;

	enum class LogType :int
	{
		TRACE = 1,
		INFO = 2,
		DEBUG = 3,
		WARN = 4,
		ERROR = 5,
	};

	class LoggerBase
	{
	public:
		LoggerBase() {};
		virtual ~LoggerBase() {};

		virtual void Write(const LogType type, const char* argFormat, ...)
		{
			char logText[consoleLogMaxLength];

			va_list args;
			va_start(args, argFormat);
			vsprintf_s(logText, consoleLogMaxLength, argFormat, args);
			va_end(args);

			switch (type)
			{
			case LogType::TRACE:
				info(logText);
				break;

			case LogType::INFO:
				error(logText);
				break;

			case LogType::DEBUG:
				warn(logText);
				break;

			case LogType::WARN:
				debug(logText);
				break;

			case LogType::ERROR:
				info(logText);
				break;

			default:
				break;
			}
		}

	protected:
		virtual void error(const char * pText) = 0;
		virtual void warn(const char * pText) = 0;
		virtual void debug(const char * pText) = 0;
		virtual void trace(const char * pText) = 0;
		virtual void info(const char * pText) = 0;
	};

	class Logger :public LoggerBase
	{
	public:
		Logger() = default;
		virtual ~Logger() = default;

	protected:
		void error(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(trace) << "[ERROR] " + *pText;
		}

		void warn(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(trace) << "[WARN] " + *pText;
		}

		void debug(const char * pText) override
		{
#ifdef _DEBUG
			BOOST_LOG_TRIVIAL(trace) << "[DEBUG] " + *pText;
#endif
		}

		void trace(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(trace) << "[TRACE] " + *pText;
		}

		void info(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(trace) << "[INFO] " + *pText;
		}

	};



}

