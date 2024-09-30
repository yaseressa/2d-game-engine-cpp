#pragma once
#include <sol/sol.hpp>
using namespace sol;
struct ScriptComponent {
	sol::function func;
	ScriptComponent(sol::function func = lua_nil) : func(func) {};
};