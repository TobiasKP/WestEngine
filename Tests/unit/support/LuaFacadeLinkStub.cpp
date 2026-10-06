// Link-time stand-in for the two LuaFacade symbols EventDispatcher.cpp references
// from EventDispatcher::init(). The tests never call init() (it needs a live Lua
// state behind the LuaFacade singleton), so reaching either function is a test bug.
// Linking the real LuaFacade.cpp would drag in PathUtils/CoreConstants/the API
// file loader for nothing.
#include <Scripting/LuaFacade.hpp>

#include <cstdio>
#include <cstdlib>

LuaFacade& LuaFacade::getLuaFacadeInstance()
{
  std::fputs("LuaFacadeLinkStub: LuaFacade::getLuaFacadeInstance() called from a unit test\n", stderr);
  std::abort();
}

bool LuaFacade::registerCFunction(int (*)(lua_State*), std::string, void*)
{
  std::fputs("LuaFacadeLinkStub: LuaFacade::registerCFunction() called from a unit test\n", stderr);
  std::abort();
}
