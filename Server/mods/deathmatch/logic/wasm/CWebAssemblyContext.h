/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyContext.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_WEB_ASSEMBLY_CONTEXT_HEADER
#define C_WEB_ASSEMBLY_CONTEXT_HEADER

#include <vector>

#include "CResource.h"

#include "WebAssemblyImports.h"

class CWebAssemblyFunction;
typedef CFastHashMap<SString, CWebAssemblyFunction*> CWebAssemblyFunctionMap;

enum class CWebAssemblyLoadState
{
    Succeed,
    Failed
};

struct CWebAssemblyExtern
{
    CWebAssemblyExternContext context;
    wasm_externkind_t         kind;
};

typedef CFastHashMap<SString, CWebAssemblyExtern> CWebAssemblyExternMap;

class CWebAssemblyEngine
{
public:
    CWebAssemblyEngine();
    CWebAssemblyEngine(const CWebAssemblyEngineContext& context);
    ~CWebAssemblyEngine();

    void Build();
    void Destroy();

    void                      SetContext(const CWebAssemblyEngineContext& context);
    CWebAssemblyEngineContext GetContext();

    operator bool();

private:
    CWebAssemblyEngineContext m_pContext;
};

class CWebAssemblyStore
{
public:
    CWebAssemblyStore();
    CWebAssemblyStore(const CWebAssemblyStoreContext& context);
    CWebAssemblyStore(CWebAssemblyEngine* engine);
    ~CWebAssemblyStore();

    void Build(CWebAssemblyEngine* engine);
    void Build();
    void Destroy();

    void                     SetContext(const CWebAssemblyStoreContext& context);
    CWebAssemblyStoreContext GetContext();

    void                SetEngine(CWebAssemblyEngine* engine);
    CWebAssemblyEngine* GetEngine();

    operator bool();

private:
    CWebAssemblyStoreContext m_pContext;

    CWebAssemblyEngine* m_waEngine;
};

class CWebAssemblyContext
{
public:
    CWebAssemblyContext();
    CWebAssemblyContext(CResource* resource);
    ~CWebAssemblyContext();

    void Destroy();

    CWebAssemblyScript*   CreateScript();
    CWebAssemblyLoadState LoadScriptBinary(CWebAssemblyScript* script, const char* binary, const size_t& binarySize, const SString& fileName);

    CWebAssemblyScriptList& GetScripts();
    void                    ClearScripts();

    void       SetResource(CResource* resource);
    CResource* GetResource();

    CWebAssemblyStore* GetStore();

    static void                InitializeWebAssemblyEngine();
    static void                DeleteWebAssemblyEngine();
    static CWebAssemblyEngine* GetWebAssemblyEngine();

    static bool IsWebAssemblyBinary(const char* binary);

private:
    CResource*         m_pResource;
    CWebAssemblyStore* m_pStore;

    CWebAssemblyScriptList m_lsScripts;
};

class CWebAssemblyFunctionType;

class CWebAssemblyScript
{
public:
    CWebAssemblyScript();
    CWebAssemblyScript(CWebAssemblyContext* context);
    ~CWebAssemblyScript();

    void Destroy();

    void CallMainFunction(int32_t argc = 0, char** argv = NULL);

    CWebAssemblyLoadState LoadBinary(const char* binary, const size_t& binarySize, const SString& scriptFile);

    void RegisterApiFunction(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);
    void RegisterGlobalFunctions(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);

    CWebAssemblyModuleContext   GetModule();
    CWebAssemblyInstanceContext GetInstance();

    CWebAssemblyExternMap& GetExports();

    CWebAssemblyExtern GetExport(const SString& exportName);

    CWebAssemblyFunctionMap& GetApiFunctions();
    CWebAssemblyFunctionMap& GetGlobalFunctions();

    bool DoesExportExist(const SString& exportName);

    bool DoesApiFunctionExist(const SString& functionName);
    bool DoesGlobalFunctionExist(const SString& functionName);

    CWebAssemblyFunction* GetApiFunction(const SString& functionName);
    CWebAssemblyFunction* GetGlobalFunction(const SString& functionName);

    void DeleteApiFunction(const SString& functionName);
    void DeleteGlobalFunction(const SString& functionName);

    void ClearApiFunctions();
    void ClearGlobalFunctions();

    SString GetScriptFile();

    static bool IsExternValid(const CWebAssemblyExtern& waExtern);

private:
    SString m_strScriptFile;

    CWebAssemblyContext* m_pContextStore;

    CWebAssemblyModuleContext   m_pModule;
    CWebAssemblyInstanceContext m_pInstance;

    CWebAssemblyExternMap m_mpExports;

    CWebAssemblyFunctionMap m_mpApiFunctions;
    CWebAssemblyFunctionMap m_mpGlobalFunctions;
};

#endif
