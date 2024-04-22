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

typedef std::vector<CWebAssemblyFunction*> CWebAssemblyInternalFunctions;

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

class CWebAssemblyVariables;
class CWebAssemblyFunctionType;
class CWebAssemblyMemory;

class CWebAssemblyScript
{
public:
    CWebAssemblyScript();
    CWebAssemblyScript(CWebAssemblyContext* context);
    ~CWebAssemblyScript();

    void Destroy();

    void CallMainFunction(int32_t argc = 0, char** argv = NULL);

    void CallInternalFunction(const size_t& index, CWebAssemblyVariables* args, CWebAssemblyVariables* results);

    CWebAssemblyLoadState LoadBinary(const char* binary, const size_t& binarySize, const SString& scriptFile);

    void RegisterApiFunction(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);
    void RegisterGlobalFunctions(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);

    CWebAssemblyContext* GetStoreContext();

    CWebAssemblyModuleContext   GetModule();
    CWebAssemblyInstanceContext GetInstance();

    void BuildExportedFunctions();
    void BuildInternalFunctions();
    void BuildMemory();

    CWebAssemblyExternMap&   GetExports();
    CWebAssemblyFunctionMap& GetExportedFunctions();

    CWebAssemblyExtern    GetExport(const SString& exportName);
    CWebAssemblyFunction* GetExportedFunction(const SString& functionName);

    size_t GetInternalFunctionsCount();

    CWebAssemblyFunctionMap& GetApiFunctions();
    CWebAssemblyFunctionMap& GetGlobalFunctions();

    CWebAssemblyMemory* GetMemory();

    bool DoesExportExist(const SString& exportName);
    bool DoesExportedFunctionExist(const SString& functionName);

    bool DoesApiFunctionExist(const SString& functionName);
    bool DoesGlobalFunctionExist(const SString& functionName);

    CWebAssemblyFunction* GetApiFunction(const SString& functionName);
    CWebAssemblyFunction* GetGlobalFunction(const SString& functionName);

    CWebAssemblyFunction* GetInternalFunction(const size_t& index);

    void DeleteExportedFunction(const SString& functionName);

    void DeleteApiFunction(const SString& functionName);
    void DeleteGlobalFunction(const SString& functionName);

    void ClearExportedFunctions();

    void ClearInternalFunctions();

    void ClearApiFunctions();
    void ClearGlobalFunctions();

    void DeleteMemory();

    SString GetScriptFile();

    static bool IsExternValid(const CWebAssemblyExtern& waExtern);

private:
    SString m_strScriptFile;

    CWebAssemblyContext* m_pContextStore;

    CWebAssemblyModuleContext   m_pModule;
    CWebAssemblyInstanceContext m_pInstance;

    CWebAssemblyExternMap   m_mpExports;
    CWebAssemblyFunctionMap m_mpExportedFunctions;

    CWebAssemblyFunctionMap m_mpApiFunctions;
    CWebAssemblyFunctionMap m_mpGlobalFunctions;

    CWebAssemblyInternalFunctions m_lsInternalFunctions;

    CWebAssemblyMemory* m_pMemory;
};

class CWebAssemblyMemory
{
public:
    CWebAssemblyMemory();
    CWebAssemblyMemory(CWebAssemblyScript* script);
    CWebAssemblyMemory(CWebAssemblyMemoryContext context);
    CWebAssemblyMemory(CWebAssemblyScript* script, CWebAssemblyMemoryContext context);
    ~CWebAssemblyMemory();

    void Destroy();

    CWebAssemblyMemoryPointerAddress Malloc(const size_t& size, void** physicalPointer);
    void                             Free(CWebAssemblyMemoryPointerAddress pointer);

    CWebAssemblyMemoryPointerAddress StringToUTF8(const SString& str);
    SString                          UTF8ToString(CWebAssemblyMemoryPointerAddress pointer, intptr_t size = -1);

    void*  GetBase();
    size_t GetSize();

    uintptr_t GetMemoryPhysicalPointerAddress(CWebAssemblyMemoryPointerAddress pointer);
    void*     GetMemoryPhysicalPointer(CWebAssemblyMemoryPointerAddress pointer);

    void                SetScript(CWebAssemblyScript* script);
    CWebAssemblyScript* GetScript();

    void                      SetContext(CWebAssemblyMemoryContext context);
    CWebAssemblyMemoryContext GetContext();

private:
    CWebAssemblyScript* m_pScript;

    CWebAssemblyMemoryContext m_pContext;
};

#endif
