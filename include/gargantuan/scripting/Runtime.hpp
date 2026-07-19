#include "gargantuan/instances/Instance.hpp"
#include <lua.h>

namespace gargantuan::scripting::runtime {

void PushInstance(lua_State *L, instances::Instance *instance);

void Open(lua_State *L);

int OpenEnvironment(lua_State *L);
int OpenInstance(lua_State *L);

} // namespace gargantuan::scripting::runtime
