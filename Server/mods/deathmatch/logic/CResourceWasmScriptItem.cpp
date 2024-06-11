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

        CWebAssemblyLoadState state = m_pWasmContext->LoadScriptBinary(script, &buffer.at(0), iSize, m_strShortName, false);

        lua_State* luaVM = m_resource->GetVirtualMachine()->GetVM();

        for (const std::pair<SString, CWebAssemblyFunction*>& item : script->GetExportedFunctions())
        {
            SString               name = item.first;
            CWebAssemblyFunction* function = item.second;

            if (name == WASM_MAIN_FUNCTION_NAME || name == WASM_MALLOC_FUNCTION_NAME || name == WASM_FREE_FUNCTION_NAME)
            {
                continue;
            }

            CCallable callable(function);
            callable.SetLuaResource(m_resource);
            callable.SetIsWasmFunctionState(true);

            CLuaArgument luaCallable;
            luaCallable.SetCallable(callable);

            luaCallable.Push(luaVM);

            lua_setglobal(luaVM, name.c_str());
        }

        if (state == CWebAssemblyLoadState::Succeed)
        {   
            script->CallMainFunction({ m_resource->GetName(), m_strShortName, std::to_string(m_pWasmContext->GetScripts().size()) });
        }

        return state == CWebAssemblyLoadState::Succeed;
    }

    return true;
}

bool CResourceWasmScriptItem::Stop()
{
    return true;
}
