#include "include\gargantuan\datatypes\UDim.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include <lualib.h>


namespace gargantuan
{
  int LibUDim_new(lua_State *L) {
    UDim dim = {
      (float)luaL_opt(L , luaL_checknumber , 1 , 0.0f),
      (int)luaL_opt(L , luaL_checknumber , 1 , 0),
    };
    StackValue<UDim>::Push(L , dim);
    return 1;
  }



  luaL_Reg LibUDim[]{
    {"new" , LibUDim_new},
    {nullptr , nullptr}
  }

  int OpenLibUDim(lua_State *L){
    luaL_register(L , "UDim" , LibUDim);
    return 0;
  }




} // namespace gargantuan






