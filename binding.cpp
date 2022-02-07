#include "binding.h"
#include <iostream>

namespace binding
{
    lua* lua::s_lua = nullptr;

    lua* lua::get()
    {
        if (s_lua == nullptr)
        {
            s_lua = new lua();
        }
        return s_lua;
    }

    void lua::release()
    {
        if (s_lua != nullptr)
        {
            delete s_lua;
        }
        s_lua = nullptr;
    }

    bool lua::execute(std::string filename)
    {
        if (luaL_dofile(m_state, filename.c_str()) == 0 && lua_pcall(m_state, 0, 0, 0) == 0) {
            lua_pop(m_state, lua_gettop(m_state));
            return true;
        }
        else {
            std::string error_string = lua_tostring(m_state, lua_gettop(m_state));
            lua_pop(m_state, lua_gettop(m_state));
            std::cerr << "Lua Error: " << error_string << std::endl;
            return false;
        }
    }

    bool lua::execute_interactive(std::string code)
    {
        if (luaL_loadstring(m_state, code.c_str()) == 0 && lua_pcall(m_state, 0, 0, 0) == 0) {
            lua_pop(m_state, lua_gettop(m_state));
            return true;
        }
        else {
            std::string error_string = lua_tostring(m_state, lua_gettop(m_state));
            lua_pop(m_state, lua_gettop(m_state));
            std::cerr << "Lua Error: " << error_string << std::endl;
            return false;
        }
    }

    void lua::bind(std::string namespace_name, std::vector<luaL_Reg> funcs)
    {
        lua_newtable(m_state);

        luaL_checkstack(m_state, 1, "error: too many upvalues");
        for (auto func : funcs) {
            lua_pushstring(m_state, func.name);
            lua_pushcclosure(m_state, func.func, 0);
            lua_settable(m_state, -3);
        }

        lua_setglobal(m_state, namespace_name.c_str());
    }

    void lua::bind(std::string name, int (*func_ptr) (lua_State*))
    {
        lua_pushcfunction(m_state, func_ptr);
        lua_setglobal(m_state, name.c_str());
    }

    lua::lua()
    {
        m_state = luaL_newstate();
        luaL_openlibs(m_state);
    }

    lua::~lua()
    {
        lua_close(m_state);
    }
}