/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceWasmScriptItem.cpp
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceWasmScriptItem.h"
#include "CResource.h"
#include "lua/CLuaMain.h"
#include "wasm/CWebAssemblyContext.h"
#include "wasm/CWebAssemblyVariable.h"
#include "wasmdefs/CWebAssemblyDefs.h"

#define WASM_LUA_FUNCTION_FIELD_NAME "wasm_function"

CResourceWasmScriptItem::CResourceWasmScriptItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_WASM_SCRIPT;

    m_strShortName = szShortName;

    m_pWasmContext = NULL;
}

CResourceWasmScriptItem::~CResourceWasmScriptItem()
{
}

bool CResourceWasmScriptItem::Start()
{
    m_pWasmContext = m_resource->GetResourceWebAssemblyContext();
    
    std::vector<char> buffer;
    FileLoad(m_strResourceFileName, buffer);
    unsigned int iSize = buffer.size();

    if (iSize > 0)
    {
        CWebAssemblyScript* script = m_pWasmContext->CreateScript();

        if (!script)
        {
            return false;
        }

        CWebAssemblyDefs::RegisterApi(script);

        CWebAssemblyLoadState state = m_pWasmContext->LoadScriptBinary(script, &buffer.at(0), iSize, m_strShortName);

        lua_State* luaVM = m_resource->GetVirtualMachine()->GetVM();

        for (const std::pair<SString, CWebAssemblyFunction*>& item : script->GetExportedFunctions())
        {
            SString               name = item.first;
            CWebAssemblyFunction* function = item.second;

            if (name == WASM_MAIN_FUNCTION_NAME || name == WASM_MALLOC_FUNCTION_NAME || name == WASM_FREE_FUNCTION_NAME)
            {
                continue;
            }

            /*lua_newtable(luaVM);

            lua_pushuserdata(luaVM, function);
            lua_setfield(luaVM, -2, WASM_LUA_FUNCTION_FIELD_NAME);

            lua_pushcfunction(luaVM, Lua_UTF8ToString);
            lua_setfield(luaVM, -2, "utf8ToString");

            lua_newtable(luaVM);

            lua_pushcfunction(luaVM, Lua_CallWebAssemblyFunction);
            lua_setfield(luaVM, -2, "__call");

            lua_setmetatable(luaVM, -2);

            lua_setglobal(luaVM, name.c_str());*/

            CCallable callable(function);
            callable.SetLuaResource(m_resource);
            callable.SetIsWasmFunctionState(true);

            CLuaArgument luaCallable;
            luaCallable.SetCallable(callable);

            luaCallable.Push(luaVM);

            lua_setglobal(luaVM, name.c_str());
        }

        return state == CWebAssemblyLoadState::Succeed;
    }

    return true;
}

bool CResourceWasmScriptItem::Stop()
{
    return true;
}

int CResourceWasmScriptItem::Lua_CallWebAssemblyFunction(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TTABLE)
    {
        CWebAssemblyFunction* function = NULL;

        lua_getfield(luaVM, 1, WASM_LUA_FUNCTION_FIELD_NAME);

        if (lua_type(luaVM, -1) == LUA_TLIGHTUSERDATA)
        {
            function = (CWebAssemblyFunction*)lua_touserdata(luaVM, -1);
        }

        lua_pop(luaVM, 1);

        if (!function)
        {
            return NULL;
        }

        CWebAssemblyFunctionType functionType = function->GetFunctionType();
        CWebAssemblyVariables&   functionArgs = functionType.GetArguments();
        size_t                   functionArgsCount = functionArgs.GetSize();

        CWebAssemblyScript* script = function->GetApiEnviornment()->script;
        CWebAssemblyMemory* memory = script->GetMemory();

        CWebAssemblyVariables args, results;

        int argsCount = lua_gettop(luaVM);

        std::vector<CWebAssemblyMemoryPointerAddress> strings;

        if (argsCount > 1)
        {
            for (int i = 2; i <= argsCount && (i - 2) < functionArgsCount; i++)
            {
                int wasmIndex = i - 2;

                CWebAssemblyVariableKind kind = functionArgs.Get(wasmIndex).GetType();

                if (lua_type(luaVM, i) == LUA_TNIL)
                {
                    args.Push(0);
                }
                else if (lua_type(luaVM, i) == LUA_TBOOLEAN)
                {
                    args.Push(lua_toboolean(luaVM, i));
                }
                else if (lua_type(luaVM, i) == LUA_TNUMBER)
                {
                    switch (kind)
                    {
                        case C_WASM_VARIABLE_TYPE_I32:
                            args.Push((int32_t)lua_tonumber(luaVM, i));
                            break;
                        case C_WASM_VARIABLE_TYPE_I64:
                            args.Push((int64_t)lua_tonumber(luaVM, i));
                            break;
                        case C_WASM_VARIABLE_TYPE_F32:
                            args.Push((float32_t)lua_tonumber(luaVM, i));
                            break;
                        case C_WASM_VARIABLE_TYPE_F64:
                            args.Push((float64_t)lua_tonumber(luaVM, i));
                            break;
                        default:
                            break;
                    }
                }
                else if (lua_type(luaVM, i) == LUA_TSTRING)
                {
                    if (kind == C_WASM_VARIABLE_TYPE_I32)
                    {
                        CWebAssemblyMemoryPointerAddress ptr = memory->StringToUTF8(lua_tolstring(luaVM, i, NULL));

                        strings.push_back(ptr);

                        args.Push((int32_t)ptr);
                    }
                    else
                    {
                        args.Push(0);
                    }
                }
            }
        }

        if (functionArgsCount - (argsCount - 1) > 0)
        {
            for (int i = (argsCount - 1); i < functionArgsCount; i++)
            {
                CWebAssemblyVariableKind kind = functionArgs.Get(i).GetType();

                switch (kind)
                {
                    case C_WASM_VARIABLE_TYPE_I32:
                        args.PushInt32();
                        break;
                    case C_WASM_VARIABLE_TYPE_I64:
                        args.PushInt64();
                        break;
                    case C_WASM_VARIABLE_TYPE_F32:
                        args.PushFloat32();
                        break;
                    case C_WASM_VARIABLE_TYPE_F64:
                        args.PushFloat64();
                        break;
                    default:
                        break;
                }
            }
        }

        SString errorMessage;

        if (!function->Call(&args, &results, &errorMessage))
        {
            CLogger::ErrorPrintf("%s:%s\n", script->GetResourcePath().c_str(), errorMessage.c_str());
            return 0;
        }

        for (CWebAssemblyMemoryPointerAddress& ptr : strings)
        {
            memory->Free(ptr);
        }

        for (CWebAssemblyVariable& var : results.GetList())
        {
            switch (var.GetType())
            {
                case C_WASM_VARIABLE_TYPE_I32:
                    lua_pushnumber(luaVM, var.GetInt32());
                    break;
                case C_WASM_VARIABLE_TYPE_I64:
                    lua_pushnumber(luaVM, var.GetInt64());
                    break;
                case C_WASM_VARIABLE_TYPE_F32:
                    lua_pushnumber(luaVM, var.GetFloat32());
                    break;
                case C_WASM_VARIABLE_TYPE_F64:
                    lua_pushnumber(luaVM, var.GetFloat64());
                    break;
                default:
                    break;
            }
        }

        return results.GetSize();
    }
    
    return 0;
}

int CResourceWasmScriptItem::Lua_UTF8ToString(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TTABLE && lua_type(luaVM, 2) == LUA_TNUMBER)
    {
        CWebAssemblyFunction* function = NULL;

        lua_getfield(luaVM, 1, WASM_LUA_FUNCTION_FIELD_NAME);

        if (lua_type(luaVM, -1) == LUA_TLIGHTUSERDATA)
        {
            function = (CWebAssemblyFunction*)lua_touserdata(luaVM, -1);
        }

        lua_pop(luaVM, 1);

        if (!function)
        {
            return 0;
        }

        CWebAssemblyMemory* memory = function->GetApiEnviornment()->script->GetMemory();

        lua_Integer ptrInt = lua_tointeger(luaVM, 2);

        if (ptrInt == WEB_ASSEMBLY_NULL_PTR)
        {
            return 0;
        }

        SString str = memory->UTF8ToString(ptrInt);

        lua_pushlstring(luaVM, str.c_str(), str.size());

        return 1;
    }

    return 0;
}
