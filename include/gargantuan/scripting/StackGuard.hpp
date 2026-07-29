#pragma once

#include <lua.h>
#include <lualib.h>
#include <source_location>

namespace gargantuan {
	class StackGuard {
	  public:
		StackGuard(lua_State *L, std::source_location loc = std::source_location::current());
		~StackGuard();

		StackGuard &Reserve(int count);
		StackGuard &Unreserve(int count);
		StackGuard &Expect(int count);

		StackGuard(const StackGuard &) = delete;
		StackGuard &operator=(const StackGuard &) = delete;

	  private:
		lua_State *L;
		std::source_location Source;
		int ExpectedSize;
	};
}
