/*
* lmarshal.c
* A Lua library for serializing and deserializing Lua values
* Richard Hundt <richardhundt@gmail.com>
*
* License: MIT
*
* Copyright (c) 2010 Richard Hundt
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SEEN_IDX  2
#define MAXNUMBER2STR	44

#if LUA_VERSION_NUM < 502
# define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
# define lua_rawlen lua_objlen
#endif

#if LUA_VERSION_NUM < 503

static int lua_isinteger(lua_State* L, int index) {
    int32_t x = (int32_t)lua_tointeger(L, index);
    lua_Number n = lua_tonumber(L, index);
    return ((lua_Number)x == n);
}

#endif


static int mar_encode_table(lua_State* L, luaL_Buffer* buf, int stkIdx ,size_t* idx);

static void buf_init(lua_State* L, luaL_Buffer* buf)
{
    luaL_buffinit(L, buf);
}

static int buf_write(lua_State* L, const char* str, size_t len, luaL_Buffer* buf)
{
    lua_pushlightuserdata(L, (void*)buf);
    luaL_addlstring(buf, str, len);
    lua_pop(L, 1);
    return 0;
}



static void mar_encode_value(lua_State* L, luaL_Buffer* buf, int stkidx, size_t* idx)
{
    size_t l = 0;
    int val_type = lua_type(L, stkidx);

    switch (val_type) {
    case LUA_TBOOLEAN: {
        int int_val = lua_toboolean(L, stkidx);
        buf_write(L, int_val == 0 ? "false" : "true", 1, buf);
        break;
    }
    case LUA_TSTRING: {
        buf_write(L, "\"", 1, buf);
        const char* str_val = lua_tolstring(L, stkidx, &l);
        buf_write(L, str_val, l, buf);
        buf_write(L, "\"", 1, buf);
        break;
    }
    case LUA_TNUMBER: {
        char buff[MAXNUMBER2STR];
        int len = 0;
        if (lua_isinteger(L, stkidx))
        {
            lua_Integer num_val = lua_tointeger(L, stkidx);
            len = l_sprintf(buff, MAXNUMBER2STR, LUA_INTEGER_FMT, num_val);
        }
        else
        {
            lua_Number num_val = lua_tonumber(L, stkidx);
            len = l_sprintf(buff, MAXNUMBER2STR, LUA_NUMBER_FMT, num_val);
        }
        buf_write(L, buff, len, buf);
        break;
    }
    case LUA_TTABLE: {
        int tag, ref;
        lua_pushvalue(L, stkidx);
        lua_rawget(L, SEEN_IDX);
        if (!lua_isnil(L, -1)) {
            const char* str_table = lua_tostring(L, -1);
            buf_write(L, str_table, strlen(str_table), buf);
            lua_pop(L, 1);
        }
        else {
            lua_pop(L, 1); /* pop nil */

            lua_pushvalue(L, stkidx);
            char str_table[64] = { 0 };
            snprintf(str_table, 64, "%s:0x%p", lua_typename(L, val_type), lua_topointer(L, stkidx));
            lua_pushstring(L, str_table);
            lua_rawset(L, SEEN_IDX);

            mar_encode_table(L, buf, stkidx, idx);
        }
        break;
    }
    case LUA_TFUNCTION: {
        char str_function[256] = { 0 };
        snprintf(str_function, 256, "%s:0x%p", lua_typename(L, val_type), lua_topointer(L, stkidx));
        buf_write(L, str_function, strlen(str_function), buf);
        break;
    }
    case LUA_TUSERDATA: {
        char str_userdata[256] = { 0 };
        int type = luaL_getmetafield(L, stkidx, "__name");
        if (LUA_TSTRING == type)
        {
            snprintf(str_userdata, 256, "%s:0x%p", lua_tostring(L, -1), lua_topointer(L, stkidx));
            lua_pop(L, 1);
        }
        else
        {
            snprintf(str_userdata, 256, "%s:0x%p", lua_typename(L, val_type), lua_topointer(L, stkidx));
        }
        buf_write(L, str_userdata, strlen(str_userdata), buf);
        break;
    }
    case LUA_TNIL: break;
    default:
        luaL_error(L, "invalid value type (%s)", lua_typename(L, val_type));
    }
}

static int mar_encode_table(lua_State* L, luaL_Buffer* buf, int stkidx, size_t* idx)
{
    int val_type = lua_type(L, stkidx);
    char str_table[256] = { 0 };
    snprintf(str_table, 256, "%s:0x%p", lua_typename(L, val_type), lua_topointer(L, stkidx));
    buf_write(L, str_table, strlen(str_table), buf);

    buf_write(L, "{", 1, buf);

    lua_pushvalue(L, stkidx);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        if( lua_isinteger(L, -2))
        {
            buf_write(L, "[", 1, buf);
            mar_encode_value(L, buf, -2, idx);
            buf_write(L, "]", 1, buf);
        }
        else if (lua_isstring(L, -2))
        {
            size_t l = 0;
            const char* key = lua_tolstring(L, -2, &l);
            buf_write(L, key, l, buf);
        }
        else
        {
            mar_encode_value(L, buf, -2, idx);
        }

        buf_write(L, " = ", 3, buf);

        mar_encode_value(L, buf, -1, idx);
        buf_write(L, ",", 1, buf);
       lua_pop(L, 1);
    }
    luaL_buffsub(buf, 1);
    buf_write(L, "}", 1, buf);
    lua_pop(L, 1);
    return 1;
}

static int mar_encode(lua_State* L)
{
    size_t idx, len;
    luaL_Buffer buf;

    if (lua_isnone(L, 1)) {
        lua_pushnil(L);
    }
    lua_settop(L, 1);


    lua_createtable(L, 0, 8);

    buf_init(L, &buf);

    lua_pushvalue(L, 1);
    mar_encode_value(L, &buf, -1, &idx);
    lua_pop(L, 1);

    luaL_pushresult(&buf);

    lua_remove(L, SEEN_IDX);

    return 1;
}


static int mar_clone(lua_State* L)
{
    mar_encode(L);
    lua_replace(L, 1);
    mar_decode(L);
    return 1;
}

static const luaL_Reg marshal[] =
{
    {"tostring",      mar_encode},
    {NULL,	    NULL}
};

#ifdef __cplusplus
extern "C" {
#endif
    int luaopen_marshal(lua_State* L)
    {
        luaL_newlib(L, marshal);
        return 1;
    }

#ifdef __cplusplus
}
#endif