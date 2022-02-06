#pragma once
extern "C" {
    #include <lua5.1/lua.h>
    #include <lua5.1/lualib.h>
    #include <lua5.1/lauxlib.h>
}

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <exception>

namespace binding
{
    class lua
    {
        private:
            lua();
            ~lua();
            static lua * s_lua;
            lua_State * m_state = nullptr;

        public:
            static lua * get();
            static void release();
            bool execute(std::string filename);
            bool execute_interactive(std::string code);

            void bind(std::string namespace_name, std::vector<luaL_Reg> funcs);
            void bind(std::string name, int (*func_ptr) (lua_State*));
    };
}