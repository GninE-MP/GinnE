/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceWasmScriptItem.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_RESOURCE_WASM_SCRIPT_ITEM_HEADER
#define C_RESOURCE_WASM_SCRIPT_ITEM_HEADER

#include "CResourceFile.h"

struct lua_State;

class CWebAssemblyContext;

class CResourceWasmScriptItem : public CResourceFile
{
public:
    CResourceWasmScriptItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes);
    ~CResourceWasmScriptItem();

    bool Start();
    bool Stop();

    CWebAssemblyContext* GetWebAssemblyContext() { return m_pWasmContext; }

private:
    SString m_strShortName;

    CWebAssemblyContext* m_pWasmContext;

    static int Lua_CallWebAssemblyFunction(lua_State* luaVM);
    static int Lua_UTF8ToString(lua_State* luaVM);
};

#endif
