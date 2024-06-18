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
#include <pthread.h>

#include "CResource.h"

#include "WebAssemblyImports.h"

class CWebAssemblyFunction;
typedef CFastHashMap<SString, CWebAssemblyFunction*> CWebAssemblyFunctionMap;

typedef std::vector<CWebAssemblyFunction*> CWebAssemblyInternalFunctions;

typedef pthread_mutex_t               CWebAssemblyThreadMutex;
typedef std::vector<CWebAssemblyThreadMutex*> CWebAssemblyThreadMutexList;

enum class CWebAssemblyLoadState
{
    Succeed,
    Failed
};

enum class CWebAssemblyThreadState
{
    Off = 0,
    Starting = 1,
    Running = 2,
    Waiting = 3,
    Terminated = 4,
    Finished = 5
};

struct CWebAssemblyExtern
{
    CWebAssemblyExternContext context;
    wasm_externkind_t         kind;
};

typedef CFastHashMap<SString, CWebAssemblyExtern> CWebAssemblyExternMap;

class CWebAssemblyScript;
class CWebAssemblyStore;

struct CWebAssemblySharedScriptData
{
    CWebAssemblyScript* script;
    CWebAssemblyStore*  store;
    pthread_t           threadId;
};

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

struct CWebAssemblyThreadData
{
    CWebAssemblyThread* thread;
    CWebAssemblyScript* mainScript;
    uint32_t            functionIndex;
    void*               data;
};

class CWebAssemblyThread
{
public:
    CWebAssemblyThread();
    CWebAssemblyThread(CWebAssemblyContext* context, CWebAssemblyThreadData data);
    ~CWebAssemblyThread();

    void Drop();

    bool Start();
    bool Terminate();
    bool Join();
    void SleepFor(uint32_t milliSeconds);

    void SetId(pthread_t threadId);
    void SetState(CWebAssemblyThreadState state);
    void SetData(CWebAssemblyThreadData data);

    void SetWasmContext(CWebAssemblyContext* context);
    void SetWasmStore(CWebAssemblyStore* store);
    void SetExecutorScript(CWebAssemblyScript* script);

    pthread_t               GetId();
    CWebAssemblyThreadState GetState();
    CWebAssemblyThreadData  GetData();
    uint32_t                GetSleepingTime();

    CWebAssemblyContext*          GetWasmContext();
    CWebAssemblyStore*            GetWasmStore();
    CWebAssemblyScript*           GetExecutorScript();

    void* GetThreadArg();

    static bool IsValidThreadData(CWebAssemblyThreadData data);

private:
    pthread_t               m_ThreadId;
    CWebAssemblyThreadState m_ThreadState;
    CWebAssemblyThreadData  m_ThreadData;
    uint32_t                m_uiSleepedFor;

    CWebAssemblyContext*          m_pContext;
    CWebAssemblyStore*            m_pStore;
    CWebAssemblyScript*           m_pExecutorScript;

    void* m_pThreadArg;

    static void* ThreadExecutor(void* threadArg);
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
    CWebAssemblyLoadState LoadScriptBinary(CWebAssemblyScript* script, CWebAssemblySharedScriptData* sharedData, bool executeMain = true);
    void                  DestroyScript(CWebAssemblyScript* script);

    CWebAssemblyThread* CreateThread(CWebAssemblyThreadData threadData);
    void                DestroyThread(CWebAssemblyThread* thread);

    CWebAssemblyThreadMutex* CreateThreadMutex();
    bool                     DestroyThreadMutex(CWebAssemblyThreadMutex* mutex);
    bool                     LockMutex(CWebAssemblyThreadMutex* mutex);
    bool                     UnlockMutex(CWebAssemblyThreadMutex* mutex);

    CWebAssemblyScriptList&      GetScripts();
    CWebAssemblyThreadList&      GetThreads();
    CWebAssemblyThreadMutexList& GetThreadMutexes();
    void                         ClearScripts();
    void                         ClearThreads();
    void                         ClearThreadMutexes();

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

    CWebAssemblyThread* FindThreadByThreadId(pthread_t threadId);

    bool DoesOwnScript(CWebAssemblyScript* script);
    bool DoesOwnThread(CWebAssemblyThread* thread);
    bool DoesOwnThreadMutex(CWebAssemblyThreadMutex* mutex);

    static void                InitializeWebAssemblyEngine(CGame* pGameObject);
    static void                DeleteWebAssemblyEngine();
    static CWebAssemblyEngine* GetWebAssemblyEngine();
    static CGame*              GetGameObject();
    static CWebAssemblyThread* GetMainThread();

    static bool IsWebAssemblyBinary(const char* binary);

private:
    CResource*         m_pResource;
    CWebAssemblyStore* m_pStore;

    CWebAssemblyScriptList      m_lsScripts;
    CWebAssemblyThreadList      m_lsThreads;
    CWebAssemblyThreadMutexList m_lsThreadMutexes;

    CWebAssemblyFunctionMap m_mpGlobalFunctions;

    static int WasmUTF8ToString(lua_State* luaVM);
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
    bool CallInternalFunctionByHash(const uintptr_t& hash, CWebAssemblyVariables* args, CWebAssemblyVariables* results);

    CWebAssemblyLoadState LoadBinary(const char* binary, const size_t& binarySize, const SString& scriptFile, CWebAssemblySharedScriptData* sharedScriptData = NULL);

    void RegisterApiFunction(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function);
    void RegisterGlobalFunction(const SString& functionName, CWebAssemblyFunction* function, bool isLuaFunction = false);

    CWebAssemblyContext* GetStoreContext();
    CWebAssemblyStore*   GetStore();

    CWebAssemblyModuleContext       GetModule();
    CWebAssemblySharedModuleContext GetSharedModule();
    CWebAssemblyInstanceContext     GetInstance();

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
    CWebAssemblyFunction* GetInternalFunctionByHash(const uintptr_t& hash);
    size_t                GetInternalFunctionIndexByHash(const uintptr_t& hash);

    uintptr_t GetInternalFunctionHash(const size_t& index);
    
    bool IsValidInternalFunctionHash(const uintptr_t& hash);

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

    CWebAssemblySharedScriptData* CreateSharedScriptData(CWebAssemblyStore* store = NULL, pthread_t* threadId = NULL);

    static bool IsExternValid(const CWebAssemblyExtern& waExtern);

    static void DeleteSharedScriptData(CWebAssemblySharedScriptData* sharedScriptData);

private:
    SString m_strScriptFile;

    CWebAssemblyContext* m_pContextStore;
    CWebAssemblyStore*   m_pStore;

    CWebAssemblyModuleContext       m_pModule;
    CWebAssemblySharedModuleContext m_pSharedModule;
    CWebAssemblyInstanceContext     m_pInstance;

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
    CWebAssemblyMemoryPointerAddress Realloc(CWebAssemblyMemoryPointerAddress pointer, const size_t& size, void** physicalPointer = NULL);
    void                             Free(CWebAssemblyMemoryPointerAddress pointer);

    CWebAssemblyMemoryPointerAddress StringToUTF8(const SString& str);
    SString                          UTF8ToString(CWebAssemblyMemoryPointerAddress pointer, intptr_t size = -1);

    void*  GetBase();
    size_t GetSize();

    uintptr_t GetMemoryPhysicalPointerAddress(CWebAssemblyMemoryPointerAddress pointer);
    void*     GetMemoryPhysicalPointer(CWebAssemblyMemoryPointerAddress pointer);

    CWebAssemblyMemoryPointerAddress GetMemoryPointerAddressByPhysicalAddress(void* pointer);

    void                SetScript(CWebAssemblyScript* script);
    CWebAssemblyScript* GetScript();

    void                      SetContext(CWebAssemblyMemoryContext context);
    CWebAssemblyMemoryContext GetContext();

    size_t GetFreeSpaceSize();

    bool DoesPointerBelongToMemory(void* ptr);

private:
    CWebAssemblyScript* m_pScript;

    CWebAssemblyMemoryContext m_pContext;
};

#endif
