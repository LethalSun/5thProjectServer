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
		WARNING = 4,
		ERR = 5,
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
				trace(logText);
				break;

			case LogType::INFO:
				info(logText);
				break;

			case LogType::DEBUG:
				debug(logText);
				break;

			case LogType::WARNING:
				warning(logText);
				break;

			case LogType::ERR:
				error(logText);
				break;

			default:
				break;
			}
		}

	protected:
		virtual void trace(const char * pText) = 0;
		virtual void info(const char * pText) = 0;
		virtual void debug(const char * pText) = 0;
		virtual void warning(const char * pText) = 0;
		virtual void error(const char * pText) = 0;


	};

	class Logger :public LoggerBase
	{
	public:
		Logger() = default;
		virtual ~Logger() = default;

	protected:
		void error(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(error) << pText;
		}

		void warning(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(warning) << pText;
		}

		void debug(const char * pText) override
		{
#ifdef _DEBUG
			BOOST_LOG_TRIVIAL(debug) << pText;
#endif
		}

		void info(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(info) << pText;
		}

		void trace(const char * pText) override
		{
			BOOST_LOG_TRIVIAL(trace) << pText;
		}

	};



}

