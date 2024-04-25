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

CResourceWasmScriptItem::CResourceWasmScriptItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_WASM_SCRIPT;

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

        CWebAssemblyLoadState state = m_pWasmContext->LoadScriptBinary(script, &buffer.at(0), iSize, m_strResourceFileName);

        return state == CWebAssemblyLoadState::Succeed;
    }

    return true;
}

bool CResourceWasmScriptItem::Stop()
{
    return true;
}
