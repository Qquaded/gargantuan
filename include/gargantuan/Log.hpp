#pragma once

#include <SDL3/SDL.h>

#define G_LOG_BASE(logger, ...) logger(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

#define G_LOG_TRACE(...) G_LOG_BASE(SDL_LogTrace, __VA_ARGS__)
#define G_LOG_VERBOSE(...) G_LOG_BASE(SDL_LogVerbose, __VA_ARGS__)
#define G_LOG_DEBUG(...) G_LOG_BASE(SDL_LogDebug, __VA_ARGS__)
#define G_LOG_INFO(...) G_LOG_BASE(SDL_LogInfo, __VA_ARGS__)
#define G_LOG_WARN(...) G_LOG_BASE(SDL_LogWarn, __VA_ARGS__)
#define G_LOG_ERROR(...) G_LOG_BASE(SDL_LogError, __VA_ARGS__)
#define G_LOG_CRITICAL(...) G_LOG_BASE(SDL_LogCritical, __VA_ARGS__)

namespace gargantuan {
	void OutputLog(void *_unused, int category, SDL_LogPriority priority, const char *message);
}
