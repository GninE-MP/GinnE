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
    CWebAssemblyLoadState LoadScriptBinary(CWebAssemblyScript* script, const char* binary, const size_t& binarySize, const SString& fileName, bool executeMain = true);

    CWebAssemblyScriptList& GetScripts();
    void                    ClearScripts();

    void       SetResource(CResource* resource);
    CResource* GetResource();

    CWebAssemblyStore* GetStore();

    void                  SetGlobalFunction(const SString& name, CWebAssemblyFunction* function);
    CWebAssemblyFunction* GetGlobalFunction(const SString& name);

    CWebAssemblyFunctionMap& GetGlobalFunctions();

    bool DoesGlobalFunctionExist(const SString& name);

    void RemoveGlobalFunction(const SString& name);
    void ClearGlobalFunctions();

    bool                DoesPointerBelongToContext(void* ptr);
    CWebAssemblyScript* FindPointerScript(void* ptr);

    static void                InitializeWebAssemblyEngine();
    static void                DeleteWebAssemblyEngine();
    static CWebAssemblyEngine* GetWebAssemblyEngine();

    static bool IsWebAssemblyBinary(const char* binary);

private:
    CResource*         m_pResource;
    CWebAssemblyStore* m_pStore;

    CWebAssemblyScriptList m_lsScripts;

    CWebAssemblyFunctionMap m_mpGlobalFunctions;
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

    void CallMainFunction(const std::vector<SString>& argv = std::vector<SString>());

    bool CallInternalFunction(const size_t& index, CWebAssemblyVariables* args, CWebAssemblyVariables* results);

    CWebAssemblyLoadState LoadBinary(const char* binary, const size_t& binarySize, const SString& scriptFile);

    void RegisterApiFunction(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);
    void RegisterGlobalFunction(const SString& functionName, CWebAssemblyFunction* function, bool isLuaFunction = false);

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

    void InsertSharedGlobalFunctions();
    void ClearSharedGlobalFunctions();

    void DeleteMemory();

    SString GetScriptFile();
    SString GetResourcePath();

    CWebAssemblyApiEnviornment CreateApiEnvironment(const SString& functionName);

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

    static WebAssemblyApi(Wasm_CallSharedImportedFunction, env, args, results);
    static WebAssemblyApi(Wasm_CallLuaImportedFunction, env, args, results);
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

    CWebAssemblyMemoryPointerAddress Malloc(const size_t& size, void** physicalPointer = NULL);
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

    bool DoesPointerBelongToMemory(void* ptr);

private:
    CWebAssemblyScript* m_pScript;

    CWebAssemblyMemoryContext m_pContext;
};

#endif
