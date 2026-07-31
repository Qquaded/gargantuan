#include "gargantuan/Log.hpp"

#include <iostream>
#include <sstream>

namespace gargantuan {
	void OutputLog(void *_unused, int category, SDL_LogPriority priority, const char *message) {
		std::ostringstream log;

		switch (category) {
		case SDL_LOG_CATEGORY_APPLICATION:
			log << "Gargantuan";
			break;
		default:
			log << "SDL";
			break;
		}

		log << "[";
		switch (priority) {
		case SDL_LOG_PRIORITY_CRITICAL:
			log << "Critical";
			break;
		case SDL_LOG_PRIORITY_ERROR:
			log << "Error";
			break;
		case SDL_LOG_PRIORITY_WARN:
			log << "Warn";
			break;
		case SDL_LOG_PRIORITY_INFO:
			log << "Info";
			break;
		case SDL_LOG_PRIORITY_DEBUG:
			log << "Debug";
			break;
		case SDL_LOG_PRIORITY_VERBOSE:
			log << "Verbose";
			break;
		case SDL_LOG_PRIORITY_TRACE:
			log << "Trace";
			break;
		case SDL_LOG_PRIORITY_COUNT:
			log << "Count";
			break;
		case SDL_LOG_PRIORITY_INVALID:
			log << "Invalid";
			break;
		}
		log << "] ";
		log << message;

		auto &output = priority >= SDL_LOG_PRIORITY_ERROR ? std::cerr : std::cout;
		output << log.str() << std::endl;
	};
}
