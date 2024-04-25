/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceWasmClientScriptItem.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_RESOURCE_WASM_CLIENT_SCRIPT_ITEM_HEADER
#define C_RESOURCE_WASM_CLIENT_SCRIPT_ITEM_HEADER

#include "CResourceFile.h"

class CResourceWasmClientScriptItem : public CResourceFile
{
public:
    CResourceWasmClientScriptItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes);
    ~CResourceWasmClientScriptItem();

    bool Start();
    bool Stop();

    bool           IsNoClientCache() const { return m_bIsNoClientCache; }
    const SString& GetSourceCode() const { return m_sourceCode; }

    ResponseCode Request(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

private:
    bool    m_bIsNoClientCache;
    SString m_sourceCode;
};

#endif
