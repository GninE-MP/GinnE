/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyContext.cpp
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLogger.h"

#include "CWebAssemblyContext.h"
#include "CWebAssemblyVariable.h"
#include "CWebAssemblyArgReader.h"

#include "../wasmdefs/CWebAssemblyDefs.h"

#include "CScriptArgReader.h"
#include "luadefs/CLuaDefs.h"

#ifndef lua_toresource
    #define lua_toresource(luaVM, index) \
        (g_pGame->GetResourceManager()->GetResourceFromScriptID(reinterpret_cast<unsigned long>((CResource*)lua_touserdata(luaVM, index))))
#endif

CGame*              pGame = NULL;
CWebAssemblyEngine* WebAssemblyEngine = NULL;
CWebAssemblyThread* WebAssemblyMainThread = NULL;

CWebAssemblyEngine::CWebAssemblyEngine()
{
    m_pContext = NULL;
}

CWebAssemblyEngine::CWebAssemblyEngine(const CWebAssemblyEngineContext& context)
{
    SetContext(context);
}

CWebAssemblyEngine::~CWebAssemblyEngine()
{
    Destroy();
}

void CWebAssemblyEngine::Build()
{
    MemAllocOption* options = new MemAllocOption;

    options->allocator.free_func = free;
    options->allocator.malloc_func = malloc;
    options->allocator.realloc_func = realloc;
    
    m_pContext = wasm_engine_new_with_args(Alloc_With_Allocator, options);
}

void CWebAssemblyEngine::Destroy()
{
    if (!m_pContext)
    {
        return;
    }

    wasm_engine_delete(m_pContext);
}

void CWebAssemblyEngine::SetContext(const CWebAssemblyEngineContext& context)
{
    m_pContext = context;
}

CWebAssemblyEngineContext CWebAssemblyEngine::GetContext()
{
    return m_pContext;
}

CWebAssemblyEngine::operator bool()
{
    return m_pContext ? true : false;
}

CWebAssemblyStore::CWebAssemblyStore()
{
    m_pContext = NULL;
}

CWebAssemblyStore::CWebAssemblyStore(const CWebAssemblyStoreContext& context)
{
    SetContext(context);
}

CWebAssemblyStore::CWebAssemblyStore(CWebAssemblyEngine* engine)
{
    SetEngine(engine);
}

CWebAssemblyStore::~CWebAssemblyStore()
{
    Destroy();
}

void CWebAssemblyStore::Build(CWebAssemblyEngine* engine)
{
    SetEngine(engine);

    Build();
}

void CWebAssemblyStore::Build()
{
    if (!m_waEngine)
    {
        return;
    }

    m_pContext = wasm_store_new(m_waEngine->GetContext());
}

void CWebAssemblyStore::Destroy()
{
    if (!m_pContext)
    {
        return;
    }

    wasm_store_delete(m_pContext);

    m_pContext = NULL;
}

void CWebAssemblyStore::SetContext(const CWebAssemblyStoreContext& context)
{
    m_pContext = context;
}

CWebAssemblyStoreContext CWebAssemblyStore::GetContext()
{
    return m_pContext;
}

void CWebAssemblyStore::SetEngine(CWebAssemblyEngine* engine)
{
    m_waEngine = engine;
}

CWebAssemblyEngine* CWebAssemblyStore::GetEngine()
{
    return m_waEngine;
}

CWebAssemblyStore::operator bool()
{
    return m_pContext ? true : false;
}

CWebAssemblyThread::CWebAssemblyThread()
{
    Drop();
}

CWebAssemblyThread::CWebAssemblyThread(CWebAssemblyContext* context, CWebAssemblyThreadData data)
{
    Drop();

    SetWasmContext(context);
    SetData(data);
}

CWebAssemblyThread::~CWebAssemblyThread()
{
    Terminate();

    if (m_pExecutorScript)
    {
        m_pExecutorScript->GetStoreContext()->DestroyScript(m_pExecutorScript);
        m_pExecutorScript = NULL;
    }

    if (m_pStore)
    {
        delete m_pStore;
        m_pStore = NULL;
    }

    Drop();
}

void CWebAssemblyThread::Drop()
{
    memset(this, 0, sizeof(CWebAssemblyThread));
}

bool CWebAssemblyThread::Start()
{
    if (m_ThreadState != CWebAssemblyThreadState::Off)
    {
        return false;
    }

    if (!m_pContext || !IsValidThreadData(m_ThreadData))
    {
        return false;
    }

    int result = pthread_create(&m_ThreadId, NULL, ThreadExecutor, &m_ThreadData);

    SetState(result == 0 ? CWebAssemblyThreadState::Starting : CWebAssemblyThreadState::Off);

    return true;
}

bool CWebAssemblyThread::Terminate()
{
    if (m_ThreadState != CWebAssemblyThreadState::Running && m_ThreadState != CWebAssemblyThreadState::Waiting && m_ThreadState != CWebAssemblyThreadState::Finished)
    {
        return false;
    }

    if (m_ThreadId.p == WebAssemblyMainThread->GetId().p)
    {
        return false;
    }

    int result = pthread_cancel(m_ThreadId);

    SetState(CWebAssemblyThreadState::Terminated);

    if (m_pExecutorScript)
    {
        m_pExecutorScript->GetStoreContext()->DestroyScript(m_pExecutorScript);
        m_pExecutorScript = NULL;
    }

    if (m_pStore)
    {
        delete m_pStore;
        m_pStore = NULL;
    }

    return result == 0 || result == 3;
}

bool CWebAssemblyThread::Join()
{
    if (m_ThreadState != CWebAssemblyThreadState::Starting && m_ThreadState != CWebAssemblyThreadState::Running && m_ThreadState != CWebAssemblyThreadState::Waiting)
    {
        return false;
    }

    return pthread_join(m_ThreadId, NULL) == 0;
}

void CWebAssemblyThread::SleepFor(uint32_t milliSeconds)
{
    if (m_ThreadState != CWebAssemblyThreadState::Running)
    {
        return;
    }

    SetState(CWebAssemblyThreadState::Waiting);

    m_uiSleepedFor = milliSeconds;

    Sleep(milliSeconds);

    SetState(CWebAssemblyThreadState::Running);

    m_uiSleepedFor = 0;
}

void CWebAssemblyThread::SetId(pthread_t threadId)
{
    m_ThreadId = threadId;
}

void CWebAssemblyThread::SetState(CWebAssemblyThreadState state)
{
    m_ThreadState = state;
}

void CWebAssemblyThread::SetData(CWebAssemblyThreadData data)
{
    m_ThreadData = data;
}

void CWebAssemblyThread::SetWasmContext(CWebAssemblyContext* context)
{
    m_pContext = context;
}

void CWebAssemblyThread::SetWasmStore(CWebAssemblyStore* store)
{
    m_pStore = store;
}

void CWebAssemblyThread::SetExecutorScript(CWebAssemblyScript* script)
{
    m_pExecutorScript = script;
}

pthread_t CWebAssemblyThread::GetId()
{
    return m_ThreadId;
}

CWebAssemblyThreadState CWebAssemblyThread::GetState()
{
    return m_ThreadState;
}

CWebAssemblyThreadData CWebAssemblyThread::GetData()
{
    return m_ThreadData;
}

uint32_t CWebAssemblyThread::GetSleepingTime()
{
    return m_uiSleepedFor;
}

CWebAssemblyContext* CWebAssemblyThread::GetWasmContext()
{
    return m_pContext;
}

CWebAssemblyStore* CWebAssemblyThread::GetWasmStore()
{
    return m_pStore;
}

CWebAssemblyScript* CWebAssemblyThread::GetExecutorScript()
{
    return m_pExecutorScript;
}

void* CWebAssemblyThread::GetThreadArg()
{
    return m_pThreadArg;
}

bool CWebAssemblyThread::IsValidThreadData(CWebAssemblyThreadData data)
{
    return data.mainScript ? true : false;
}

void* CWebAssemblyThread::ThreadExecutor(void* threadArg)
{
    CWebAssemblyThreadData* data = (CWebAssemblyThreadData*)threadArg;

    if (!data)
    {
        pthread_exit(NULL);
        return NULL;
    }

    int result;

    result = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (result != 0)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        pthread_exit(NULL);

        return NULL;
    }

    result = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if (result != 0)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        pthread_exit(NULL);

        return NULL;
    }

    CWebAssemblyEngine*  engine = CWebAssemblyContext::GetWebAssemblyEngine();
    CWebAssemblyContext* context = data->thread->GetWasmContext();

    CWebAssemblyStore* workerStore = new CWebAssemblyStore(engine);

    if (!workerStore)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        pthread_exit(NULL);

        return NULL;
    }

    data->thread->SetWasmStore(workerStore);

    workerStore->Build();

    if (!*workerStore)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        pthread_exit(NULL);

        return NULL;
    }

    CWebAssemblySharedScriptData* sharedData = data->mainScript->CreateSharedScriptData(workerStore, &(data->thread->GetId()));

    if (!sharedData)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        pthread_exit(NULL);

        return NULL;
    }

    CWebAssemblyScript* workerScript = context->CreateScript();

    if (!workerScript)
    {
        data->thread->SetState(CWebAssemblyThreadState::Finished);

        CWebAssemblyScript::DeleteSharedScriptData(sharedData);

        pthread_exit(NULL);

        return NULL;
    }

    data->thread->SetExecutorScript(workerScript);

    CWebAssemblyDefs::RegisterApi(workerScript);

    CWebAssemblyLoadState loadState = context->LoadScriptBinary(workerScript, sharedData, false);

    if (loadState == CWebAssemblyLoadState::Succeed)
    {
        CWebAssemblyVariables args;

        args.Push((intptr_t)data->data);

        data->thread->SetState(CWebAssemblyThreadState::Running);

        workerScript->CallInternalFunction(data->functionIndex, &args, NULL);
    }

    CWebAssemblyScript::DeleteSharedScriptData(sharedData);

    data->thread->SetState(CWebAssemblyThreadState::Finished);

    pthread_exit(NULL);
    
    return NULL;
}

CWebAssemblyContext::CWebAssemblyContext()
{
    m_pResource = NULL;

    if (WebAssemblyEngine)
    {
        m_pStore = new CWebAssemblyStore(WebAssemblyEngine);
        m_pStore->Build();

        return;
    }

    m_pStore = NULL;
}

CWebAssemblyContext::CWebAssemblyContext(CResource* resource)
{
    SetResource(resource);

    if (WebAssemblyEngine)
    {
        m_pStore = new CWebAssemblyStore(WebAssemblyEngine);
        m_pStore->Build();

        return;
    }

    m_pStore = NULL;
}

CWebAssemblyContext::~CWebAssemblyContext()
{
    Destroy();
}

void CWebAssemblyContext::Destroy()
{
    ClearThreads();
    ClearThreadMutexes();
    ClearScripts();
    ClearGlobalFunctions();

    if (m_pStore)
    {
        delete m_pStore;
    }

    m_pStore = NULL;
}

CWebAssemblyScript* CWebAssemblyContext::CreateScript()
{
    return new CWebAssemblyScript(this);
}

CWebAssemblyLoadState CWebAssemblyContext::LoadScriptBinary(CWebAssemblyScript* script, const char* binary, const size_t& binarySize, const SString& fileName, bool executeMain)
{
    if (!script)
    {
        return CWebAssemblyLoadState::Failed;
    }

    CWebAssemblyLoadState state = script->LoadBinary(binary, binarySize, fileName);

    if (state == CWebAssemblyLoadState::Succeed)
    {
        m_lsScripts.push_back(script);

        if (executeMain)
        {
            script->CallMainFunction({GetResource()->GetName(), fileName, std::to_string(m_lsScripts.size())});
        }
    }

    return state;
}

CWebAssemblyLoadState CWebAssemblyContext::LoadScriptBinary(CWebAssemblyScript* script, CWebAssemblySharedScriptData* sharedData, bool executeMain)
{
    if (!script)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (!sharedData || !sharedData->script)
    {
        return CWebAssemblyLoadState::Failed;
    }

    CWebAssemblyLoadState state = script->LoadBinary(NULL, 0, sharedData->script->GetScriptFile(), sharedData);

    if (state == CWebAssemblyLoadState::Succeed)
    {
        m_lsScripts.push_back(script);

        if (executeMain)
        {
            script->CallMainFunction({ GetResource()->GetName(), sharedData->script->GetScriptFile(), std::to_string(m_lsScripts.size()) });
        }
    }

    return state;
}

void CWebAssemblyContext::DestroyScript(CWebAssemblyScript* script)
{
    if (!DoesOwnScript(script))
    {
        return;
    }

    int index = -1;

    for (int i = 0; i < m_lsScripts.size(); i++)
    {
        if (m_lsScripts[i] == script)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        delete script;

        m_lsScripts.erase(m_lsScripts.begin() + index);
    }
}

CWebAssemblyThread* CWebAssemblyContext::CreateThread(CWebAssemblyThreadData threadData)
{
    if (!CWebAssemblyThread::IsValidThreadData(threadData))
    {
        return NULL;
    }

    CWebAssemblyThread* thread = new CWebAssemblyThread(this, threadData);

    if (!thread)
    {
        return NULL;
    }

    m_lsThreads.push_back(thread);

    return thread;
}

void CWebAssemblyContext::DestroyThread(CWebAssemblyThread* thread)
{
    if (!DoesOwnThread(thread))
    {
        return;
    }

    int index = -1;

    for (int i = 0; i < m_lsThreads.size(); i++)
    {
        if (m_lsThreads[i] == thread)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        delete thread;

        m_lsThreads.erase(m_lsThreads.begin() + index);
    }
}

CWebAssemblyThreadMutex* CWebAssemblyContext::CreateThreadMutex()
{
    CWebAssemblyThreadMutex* mutex = new CWebAssemblyThreadMutex();

    if (!mutex)
    {
        return NULL;
    }

    if (pthread_mutex_init(mutex, NULL) != 0)
    {
        delete mutex;
        return NULL;
    }

    m_lsThreadMutexes.push_back(mutex);

    return mutex;
}

bool CWebAssemblyContext::DestroyThreadMutex(CWebAssemblyThreadMutex* mutex)
{
    if (!DoesOwnThreadMutex(mutex))
    {
        return false;
    }

    bool result = pthread_mutex_destroy(mutex);

    int index = -1;

    for (int i = 0; i < m_lsThreadMutexes.size(); i++)
    {
        if (m_lsThreadMutexes[i] == mutex)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        delete mutex;

        m_lsThreadMutexes.erase(m_lsThreadMutexes.begin() + index);
    }

    return result == 0;
}

bool CWebAssemblyContext::LockMutex(CWebAssemblyThreadMutex* mutex)
{
    if (!DoesOwnThreadMutex(mutex))
    {
        return false;
    }

    return pthread_mutex_lock(mutex) == 0;
}

bool CWebAssemblyContext::UnlockMutex(CWebAssemblyThreadMutex* mutex)
{
    if (!DoesOwnThreadMutex(mutex))
    {
        return false;
    }

    return pthread_mutex_unlock(mutex) == 0;
}

CWebAssemblyScriptList& CWebAssemblyContext::GetScripts()
{
    return m_lsScripts;
}

CWebAssemblyThreadList& CWebAssemblyContext::GetThreads()
{
    return m_lsThreads;
}

CWebAssemblyThreadMutexList& CWebAssemblyContext::GetThreadMutexes()
{
    return m_lsThreadMutexes;
}

void CWebAssemblyContext::ClearScripts()
{
    if (m_lsScripts.empty())
    {
        return;
    }

    size_t length = m_lsScripts.size();

    for (size_t i = 0; i < length; i++)
    {
        CWebAssemblyScript* script = m_lsScripts[i];

        if (script)
        {
            delete script;
        }
    }

    m_lsScripts.clear();
}

void CWebAssemblyContext::ClearThreads()
{
    if (m_lsThreads.empty())
    {
        return;
    }

    size_t length = m_lsThreads.size();

    for (size_t i = 0; i < length; i++)
    {
        CWebAssemblyThread* thread = m_lsThreads[i];

        if (thread)
        {
            delete thread;
        }
    }

    m_lsThreads.clear();
}

void CWebAssemblyContext::ClearThreadMutexes()
{
    if (m_lsThreadMutexes.empty())
    {
        return;
    }

    size_t length = m_lsThreadMutexes.size();

    for (size_t i = 0; i < length; i++)
    {
        CWebAssemblyThreadMutex* mutex = m_lsThreadMutexes[i];

        if (mutex)
        {
            pthread_mutex_destroy(mutex);
        }
    }

    m_lsThreadMutexes.clear();
}

void CWebAssemblyContext::SetResource(CResource* resource)
{
    m_pResource = resource;

    if (!resource)
    {
        return;
    }

    lua_State* luaVM = resource->GetVirtualMachine()->GetVM();

    if (!luaVM)
    {
        return;
    }

    lua_pushcfunction(luaVM, WasmUTF8ToString);
    lua_setglobal(luaVM, "wasmUtf8ToString");
}

int CWebAssemblyContext::WasmUTF8ToString(lua_State* luaVM)
{
    CWebAssemblyUserData ptr;
    intptr_t             maxSize;

    CScriptArgReader argStream(luaVM);

    argStream.ReadNumber(ptr);
    argStream.ReadNumber(maxSize, -1);

    void* ptrAddress = (void*)ptr;

    if (!ptrAddress)
    {
        return 0;
    }

    CWebAssemblyContext* wasmContext = pGame->GetLuaManager()->GetVirtualMachineResource(luaVM)->GetResourceWebAssemblyContext();

    if (!wasmContext)
    {
        return 0;
    }

    CWebAssemblyScript* script = wasmContext->FindPointerScript(ptrAddress);

    if (!script)
    {
        pGame->GetScriptDebugging()->LogError(luaVM, "This memory pointer doesn't belong to this resource!");
        return 0;
    }

    CWebAssemblyMemory* memory = script->GetMemory();

    CWebAssemblyMemoryPointerAddress modulePtrAddress = memory->GetMemoryPointerAddressByPhysicalAddress(ptrAddress);

    SString data = memory->UTF8ToString(modulePtrAddress, maxSize);

    lua_pushlstring(luaVM, data.data(), data.length());

    return 1;
}

CResource* CWebAssemblyContext::GetResource()
{
    return m_pResource;
}

CWebAssemblyStore* CWebAssemblyContext::GetStore()
{
    return m_pStore;
}

void CWebAssemblyContext::SetGlobalFunction(const SString& functionName, CWebAssemblyFunction* function)
{
    if (functionName.empty())
    {
        return;
    }

    if (!function)
    {
        return;
    }

    m_mpGlobalFunctions[functionName] = function;
}

CWebAssemblyFunction* CWebAssemblyContext::GetGlobalFunction(const SString& functionName)
{
    if (!DoesGlobalFunctionExist(functionName))
    {
        return NULL;
    }

    return m_mpGlobalFunctions[functionName];
}

CWebAssemblyFunctionMap& CWebAssemblyContext::GetGlobalFunctions()
{
    return m_mpGlobalFunctions;
}

bool CWebAssemblyContext::DoesGlobalFunctionExist(const SString& functionName)
{
    return m_mpGlobalFunctions.find(functionName) != m_mpGlobalFunctions.end();
}

void CWebAssemblyContext::RemoveGlobalFunction(const SString& functionName)
{
    if (!DoesGlobalFunctionExist(functionName))
    {
        return;
    }

    m_mpGlobalFunctions.erase(functionName);
}

void CWebAssemblyContext::ClearGlobalFunctions()
{
    if (m_mpGlobalFunctions.empty())
    {
        return;
    }

    m_mpGlobalFunctions.clear();
}

bool CWebAssemblyContext::DoesPointerBelongToContext(void* ptr)
{
    if (!ptr)
    {
        return false;
    }

    if (m_lsScripts.empty())
    {
        return false;
    }

    for (CWebAssemblyScript* script : m_lsScripts)
    {
        if (script->GetMemory()->DoesPointerBelongToMemory(ptr))
        {
            return true;
        }
    }

    return false;
}

CWebAssemblyScript* CWebAssemblyContext::FindPointerScript(void* ptr)
{
    if (!ptr)
    {
        return NULL;
    }

    if (m_lsScripts.empty())
    {
        return NULL;
    }

    for (CWebAssemblyScript* script : m_lsScripts)
    {
        if (script->GetMemory()->DoesPointerBelongToMemory(ptr))
        {
            return script;
        }
    }

    return NULL;
}

CWebAssemblyThread* CWebAssemblyContext::FindThreadByThreadId(pthread_t threadId)
{
    if (!threadId.p)
    {
        return NULL;
    }

    for (CWebAssemblyThread* thread : m_lsThreads)
    {
        if (thread->GetId().p == threadId.p)
        {
            return thread;
        }
    }

    return NULL;
}

bool CWebAssemblyContext::DoesOwnScript(CWebAssemblyScript* script)
{
    if (!script || script->GetStoreContext() != this || m_lsScripts.empty())
    {
        return false;
    }

    for (CWebAssemblyScript* s : m_lsScripts)
    {
        if (s == script)
        {
            return true;
        }
    }

    return false;
}

bool CWebAssemblyContext::DoesOwnThread(CWebAssemblyThread* thread)
{
    if (!thread || thread->GetWasmContext() != this || m_lsThreads.empty())
    {
        return false;
    }

    for (CWebAssemblyThread* th : m_lsThreads)
    {
        if (th == thread)
        {
            return true;
        }
    }

    return false;
}

bool CWebAssemblyContext::DoesOwnThreadMutex(CWebAssemblyThreadMutex* mutex)
{
    if (!mutex || m_lsThreadMutexes.empty())
    {
        return false;
    }

    for (CWebAssemblyThreadMutex* mut : m_lsThreadMutexes)
    {
        if (mut == mutex)
        {
            return true;
        }
    }

    return false;
}

void CWebAssemblyContext::InitializeWebAssemblyEngine(CGame* pGameObject)
{
    if (WebAssemblyEngine)
    {
        DeleteWebAssemblyEngine();
    }

    pGame = pGameObject;

    CLogger::LogPrintf("Building new web assembly engine...\n");

    WebAssemblyEngine = new CWebAssemblyEngine();
    WebAssemblyEngine->Build();

    CWebAssemblyDefs::RegisterApiFunctionTypes();

    WebAssemblyMainThread = new CWebAssemblyThread();

    memset(WebAssemblyMainThread, 0, sizeof(CWebAssemblyThread));

    CWebAssemblyThreadData threadData;
    memset(&threadData, 0, sizeof(threadData));

    threadData.thread = WebAssemblyMainThread;

    WebAssemblyMainThread->SetId(pthread_self());
    WebAssemblyMainThread->SetState(CWebAssemblyThreadState::Running);
    WebAssemblyMainThread->SetData(threadData);

    CLogger::LogPrintf("Web assembly engine built.\n");
}

void CWebAssemblyContext::DeleteWebAssemblyEngine()
{
    if (!WebAssemblyEngine)
    {
        return;
    }

    WebAssemblyEngine->Destroy();

    delete WebAssemblyEngine;
    WebAssemblyEngine = NULL;

    delete WebAssemblyMainThread;
    WebAssemblyMainThread = NULL;
}

CWebAssemblyEngine* CWebAssemblyContext::GetWebAssemblyEngine()
{
    return WebAssemblyEngine;
}

CGame* CWebAssemblyContext::GetGameObject()
{
    return pGame;
}

CWebAssemblyThread* CWebAssemblyContext::GetMainThread()
{
    return WebAssemblyMainThread;
}

bool CWebAssemblyContext::IsWebAssemblyBinary(const char* binary)
{
    if (!binary)
    {
        return false;
    }

    const size_t WASM_BINARY_HEADER_SIZE = 5;
    
	unsigned char wasmHeader[WASM_BINARY_HEADER_SIZE] = {0, 97, 115, 109, 1};

    return std::string(binary, WASM_BINARY_HEADER_SIZE) == std::string((char*)wasmHeader, WASM_BINARY_HEADER_SIZE);
}

CWebAssemblyScript::CWebAssemblyScript()
{
    m_pContextStore = NULL;

    m_pModule = NULL;
    m_pSharedModule = NULL;
    m_pInstance = NULL;

    m_pMemory = NULL;
}

CWebAssemblyScript::CWebAssemblyScript(CWebAssemblyContext* context)
{
    m_pContextStore = context;

    m_pModule = NULL;
    m_pSharedModule = NULL;
    m_pInstance = NULL;

    m_pMemory = NULL;
}

CWebAssemblyScript::~CWebAssemblyScript()
{
    Destroy();
}

void CWebAssemblyScript::CallMainFunction(const std::vector<SString>& argv)
{
    CWebAssemblyFunction* mainFunction = GetExportedFunction(WASM_MAIN_FUNCTION_NAME);

    CWebAssemblyVariables args;
    CWebAssemblyVariables results;

    void*                                         argvs = NULL;
    CWebAssemblyMemoryPointerAddress              argvAddress = WEB_ASSEMBLY_NULL_PTR;
    std::vector<CWebAssemblyMemoryPointerAddress> argvAddressList;

    SString errorMessage;

    if (!mainFunction)
    {
        goto Fail;
    }

    size_t argsLength = argv.size();

    args.Push((int32_t)argsLength);

    if (argsLength > 0)
    {
        argvAddress = m_pMemory->Malloc(argsLength * sizeof(CWebAssemblyMemoryPointerAddress), &argvs);

        CWebAssemblyMemoryPointerAddress* argvList = (CWebAssemblyMemoryPointerAddress*)argvs;

        for (size_t i = 0; i < argsLength; i++)
        {
            argvList[i] = m_pMemory->StringToUTF8(argv[i]);
        }

        args.Push((int32_t)argvAddress);
    }
    else
    {
        args.Push((int32_t)WEB_ASSEMBLY_NULL_PTR);
    }

    if (!mainFunction->Call(&args, &results, &errorMessage))
    {
        goto Fail;
    }

    if (results.GetSize() > 0)
    {
        int32_t exitCode = results.GetFirst().GetInt32();

        if (exitCode)
        {
            CLogger::ErrorPrintf("Web assembly module existed with code : %d\n", exitCode);
        }
    }

    goto CleanUp;

Fail:
    if (!errorMessage.empty())
    {
        CLogger::ErrorPrintf("%s:%s\n", GetResourcePath().c_str(), errorMessage.c_str());

        goto CleanUp;
    }

    if (mainFunction)
    {
        CLogger::ErrorPrintf("Couldn't call web assembly module function '%s'.\n", mainFunction->GetFunctionType().GenerateFunctionStructureText(WASM_MAIN_FUNCTION_NAME).c_str());
        goto CleanUp;
    }

    CLogger::ErrorPrintf("Couldn't call web assembly '%s' function.\n", WASM_MAIN_FUNCTION_NAME);

CleanUp:
    if (argvs)
    {
        CWebAssemblyMemoryPointerAddress* argvList = (CWebAssemblyMemoryPointerAddress*)argvs;

        for (size_t i = 0; i < argsLength; i++)
        {
            m_pMemory->Free(argvList[i]);
        }

        m_pMemory->Free(argvAddress);
    }
}

bool CWebAssemblyScript::CallInternalFunction(const size_t& index, CWebAssemblyVariables* args, CWebAssemblyVariables* results)
{
    CWebAssemblyFunction* function = GetInternalFunction(index);

    if (!function)
    {
        return false;
    }

    return function->Call(args, results);
}

bool CWebAssemblyScript::CallInternalFunctionByHash(const uintptr_t& hash, CWebAssemblyVariables* args, CWebAssemblyVariables* results)
{
    CWebAssemblyFunction* function = GetInternalFunctionByHash(hash);

    if (!function)
    {
        return false;
    }

    return function->Call(args, results);
}

void CWebAssemblyScript::Destroy()
{
    ClearSharedGlobalFunctions();

    ClearExportedFunctions();
    ClearInternalFunctions();
    ClearApiFunctions();
    ClearGlobalFunctions();

    DeleteMemory();

    m_mpExports.clear();

    if (m_pInstance)
    {
        wasm_instance_delete(m_pInstance);
    }

    if (m_pSharedModule)
    {
        wasm_shared_module_delete(m_pSharedModule);
    }

    if (m_pModule)
    {
        wasm_module_delete(m_pModule);
    }

    m_pInstance = NULL;
    m_pSharedModule = NULL;
    m_pInstance = NULL;
}

CWebAssemblyLoadState CWebAssemblyScript::LoadBinary(const char* binary, const size_t& binarySize, const SString& fileName, CWebAssemblySharedScriptData* sharedScriptData)
{
    bool isValidSharedData = sharedScriptData && sharedScriptData->script && sharedScriptData->store;

    if (!isValidSharedData)
    {
        if (binarySize < 1)
        {
            return CWebAssemblyLoadState::Failed;
        }

        if (!CWebAssemblyContext::IsWebAssemblyBinary(binary))
        {
            return CWebAssemblyLoadState::Failed;
        }
    }

    if (!m_pContextStore)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (m_pInstance || m_pModule)
    {
        Destroy();
    }

    m_strScriptFile = fileName;

    m_pStore = (
        (sharedScriptData && sharedScriptData->store) ?
        sharedScriptData->store :
        (
            (sharedScriptData && sharedScriptData->script) ?
            sharedScriptData->script->GetStore() :
            m_pContextStore->GetStore()
        )
    );

    if (!m_pStore)
    {
        return CWebAssemblyLoadState::Failed;
    }

    CWebAssemblyStoreContext store = m_pStore->GetContext();

    if (!store)
    {
        return CWebAssemblyLoadState::Failed;
    }

    std::vector<SString>                   exportNames;
    std::vector<wasm_externkind_t>         exportKinds;
    std::vector<CWebAssemblyExternContext> exports;

    CWebAssemblyExternContext* imports = NULL;
    
    wasm_exporttype_vec_t exportTypes = { 0 };

    wasm_byte_vec_t binaryBuffer;
    binaryBuffer.data = NULL;

    if (isValidSharedData)
    {
        m_pModule = wasm_module_obtain(store, sharedScriptData->script->GetSharedModule());

        if (!m_pModule)
        {
            goto Fail;
        }

        m_pSharedModule = wasm_module_share(m_pModule);
    }
    else
    {
        wasm_byte_vec_new_uninitialized(&binaryBuffer, binarySize);

        memcpy(binaryBuffer.data, binary, binarySize);

        if (!wasm_module_validate(store, &binaryBuffer))
        {
            goto Fail;
        }

        m_pModule = wasm_module_new(store, &binaryBuffer);

        wasm_byte_vec_delete(&binaryBuffer);
        binaryBuffer.data = NULL;

        if (!m_pModule)
        {
            goto Fail;
        }

        m_pSharedModule = wasm_module_share(m_pModule);
    }

    CWebAssemblyTrap* trap = NULL;

    wasm_importtype_vec_t moduleImportTypes = { 0 };
    wasm_module_imports(m_pModule, &moduleImportTypes);

    size_t moduleImportTypesLength = moduleImportTypes.num_elems;

    if (moduleImportTypesLength > 0)
    {
        imports = new CWebAssemblyExternContext[moduleImportTypesLength];

        for (size_t i = 0; i < moduleImportTypesLength; i++)
        {
            wasm_importtype_t* importType = moduleImportTypes.data[i];

            if (wasm_importtype_is_linked(importType))
            {
                imports[i] = wasm_extern_new_empty(m_pContextStore->GetStore()->GetContext(), wasm_externtype_kind(wasm_importtype_type(importType)));
                continue;
            }

            SString moduleName = wasm_importtype_module(importType)->data;
            SString importName = wasm_importtype_name(importType)->data;

            if (moduleName == WEB_ASSEMBLY_API_MODULE_NAME)
            {
                wasm_externtype_t* importExternType = (wasm_externtype_t*)wasm_importtype_type(importType);

                switch (wasm_externtype_kind(importExternType))
                {
                    case WASM_EXTERN_FUNC:
                        break;
                    case WASM_EXTERN_MEMORY:
#if WASM_COMPILER_CAN_USE_SHAERD_MEMORY
                        {
                            wasm_memorytype_t* memoryType = wasm_externtype_as_memorytype(importExternType);

                            const wasm_limits_t* limits = wasm_memorytype_limits(memoryType);

                            CWebAssemblyScript* parentScript = isValidSharedData ? sharedScriptData->script : NULL;

                            wasm_memory_t* memoryObject = parentScript ? parentScript->GetMemory()->GetContext() : wasm_memory_new(store, memoryType);

                            if (!memoryObject)
                            {
                                goto Fail;
                            }

                            if (parentScript)
                            {
                                wasm_memorytype_t* memoryObjectType = wasm_memory_type(memoryObject);

                                const wasm_limits_t* memoryObjectLimits = wasm_memorytype_limits(memoryObjectType);

                                if (memoryObjectLimits)
                                {
                                    if (limits->min != memoryObjectLimits->min || limits->max != memoryObjectLimits->max)
                                    {
                                        CLogger::ErrorPrintf("Can't import memory from other scripts with different memory size. [\"@%s/%s\"]\n", m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());

                                        goto Fail;
                                    }
                                }
                            }

                            m_pMemory = new CWebAssemblyMemory(this, memoryObject);

                            imports[i] = wasm_memory_as_extern(memoryObject);

                            continue;
                        }
#else
                        CLogger::LogPrintf("Gnine can't import wasm memory yet! but it will be able to do that in future updates. [\"@%s/%s\"]\n", m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());
                        goto Fail;
#endif

                        break;
                    case WASM_EXTERN_GLOBAL:
                    case WASM_EXTERN_TABLE:
                        goto Fail;
                        break;
                    default:
                        break;
                }

                CWebAssemblyFunctionType apiFunctionType;
                CWebAssemblyFunctionType globalFunctionType;
                CWebAssemblyFunctionType importFuncType;

                wasm_externtype_t* externType = (wasm_externtype_t*)wasm_importtype_type(importType);
                wasm_functype_t*   functype = wasm_externtype_as_functype(externType);

                importFuncType.ReadFunctionTypeContext(functype);

                if (!DoesApiFunctionExist(importName))
                {
                    if (!m_pContextStore->DoesGlobalFunctionExist(importName))
                    {
                        goto ImportFail;
                    }

                    goto CheckGlobalFunctions;
                }

                CWebAssemblyFunction* apiFunction = m_mpApiFunctions[importName];

                apiFunctionType = apiFunction->GetFunctionType();

                if (!apiFunctionType.Compare(importFuncType))
                {
                    goto CheckGlobalFunctions;
                }
                
                imports[i] = wasm_func_as_extern(apiFunction->GetFunctionContext());
                
                continue;

            CheckGlobalFunctions:
                if (!m_pContextStore->DoesGlobalFunctionExist(importName))
                {
                    if (apiFunction)
                    {
                        if (!apiFunctionType.Compare(importFuncType))
                        {
                            CLogger::ErrorPrintf("Wrong function structure on import. Defined structure is '%s' [\"@%s/%s\"]\n", apiFunctionType.GenerateFunctionStructureText(importName).c_str(), m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());
                            goto Fail;
                        }
                    }

                    goto ImportFail;
                }

                CWebAssemblyFunction* globalFunction = m_pContextStore->GetGlobalFunction(importName);

                globalFunctionType = globalFunction->GetFunctionType();

                if (!globalFunctionType.Compare(importFuncType))
                {
                    CLogger::ErrorPrintf("Wrong function structure on import. Defined structure is '%s' [\"@%s/%s\"]\n", globalFunctionType.GenerateFunctionStructureText(importName).c_str(), m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());
                    goto Fail;
                }

                RegisterGlobalFunction(importName, globalFunction, false);

                if (!DoesGlobalFunctionExist(importName))
                {
                    goto ImportFail;
                }

                globalFunction = GetGlobalFunction(importName);

                imports[i] = wasm_func_as_extern(globalFunction->GetFunctionContext());

                continue;
                
            ImportFail:
                CWebAssemblyFunction     dummyFunction;

                dummyFunction.SetFunctionType(importFuncType);
                
                RegisterGlobalFunction(importName, &dummyFunction, true);

                if (!DoesGlobalFunctionExist(importName))
                {
                    goto Fail;
                }

                CWebAssemblyFunction* luaFunctionCaller = GetGlobalFunction(importName);

                imports[i] = wasm_func_as_extern(luaFunctionCaller->GetFunctionContext());
            }
            else
            {
                imports[i] = wasm_extern_new_empty(m_pContextStore->GetStore()->GetContext(), wasm_externtype_kind(wasm_importtype_type(importType)));                
            }
        }
    }

    wasm_importtype_vec_delete(&moduleImportTypes);
    moduleImportTypes.data = NULL;

    wasm_extern_vec_t moduleImports = { sizeof(imports), imports, moduleImportTypesLength, sizeof(imports) / sizeof(*(imports)), 0 };

    m_pInstance = wasm_instance_new(store, m_pModule, &moduleImports, &trap);

    if (!m_pInstance)
    {
        goto Fail;
    }

    if (imports)
    {
        delete imports;
        imports = NULL;
    }

    wasm_module_exports(m_pModule, &exportTypes);

    size_t length = exportTypes.num_elems;

    if (length > 0 && exportTypes.data)
    {
        for (size_t i = 0; i < length; i++)
        {
            wasm_exporttype_t* exportData = exportTypes.data[i];

            const wasm_name_t* nameData = wasm_exporttype_name(exportData);

            SString name = nameData->data;

            if (!name.empty())
            {
                exportNames.push_back(name);

                const wasm_externtype_t* externType = wasm_exporttype_type(exportData);

                exportKinds.push_back(wasm_externtype_kind(externType));
            }
        }
    }

    wasm_exporttype_vec_delete(&exportTypes);
    exportTypes.data = NULL;

    wasm_extern_vec_t instanceExports = { 0 };
    wasm_instance_exports(m_pInstance, &instanceExports);
    
    length = instanceExports.num_elems;

    if (length > 0 && instanceExports.data)
    {
        for (size_t i = 0; i < length; i++)
        {
            wasm_extern_t* externData = instanceExports.data[i];

            if (externData)
            {
                exports.push_back(externData);
            }
        }
    }

    if (exportNames.size() != exports.size())
    {
        goto Fail;
    }

    length = exports.size();

    for (size_t i = 0; i < length; i++)
    {
        CWebAssemblyExtern externItem;
        externItem.context = exports[i];
        externItem.kind = exportKinds[i];

        m_mpExports[exportNames[i]] = externItem;
    }

    BuildExportedFunctions();
    BuildInternalFunctions();

    if (!m_pMemory)
    {
        BuildMemory();
    }

    InsertSharedGlobalFunctions();

    return CWebAssemblyLoadState::Succeed;

Fail:
    if (exportTypes.data)
    {
        wasm_exporttype_vec_delete(&exportTypes);
    }

    if (moduleImportTypes.data)
    {
        wasm_importtype_vec_delete(&moduleImportTypes);
    }

    if (imports)
    {
        delete imports;
        imports = NULL;
    }

    if (trap)
    {
        wasm_message_t message;
        wasm_trap_message(trap, &message);

        CLogger::ErrorPrintf("Creating new wasm module instance failed : %s\n", message.data);

        wasm_trap_delete(trap);
        wasm_name_delete(&message);
    }

    if (binaryBuffer.data)
    {
        wasm_byte_vec_delete(&binaryBuffer);
    }

    Destroy();

    return CWebAssemblyLoadState::Failed;
}

void CWebAssemblyScript::RegisterApiFunction(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function)
{
    if (functionName.empty())
    {
        return;
    }

    if (DoesApiFunctionExist(functionName))
    {
        DeleteApiFunction(functionName);
    }

    CWebAssemblyFunction* wasmFunction = new CWebAssemblyFunction(m_pContextStore->GetStore(), functionType, function, CreateApiEnvironment(functionName));
    wasmFunction->Build();

    wasmFunction->SetFunctionName(functionName);

    if (!wasmFunction || !wasmFunction->GetFunctionContext())
    {
        return;
    }

    m_mpApiFunctions[functionName] = wasmFunction;
}

void CWebAssemblyScript::RegisterGlobalFunction(const SString& functionName, CWebAssemblyFunction* function, bool isLuaFunction)
{
    if (functionName.empty())
    {
        return;
    }

    if (isLuaFunction)
    {
        if (!function)
        {
            return;
        }

        if (DoesGlobalFunctionExist(functionName))
        {
            DeleteGlobalFunction(functionName);
        }

        CWebAssemblyFunction* wasmFunction = new CWebAssemblyFunction(m_pContextStore->GetStore(), function->GetFunctionType(), Wasm_CallLuaImportedFunction, CreateApiEnvironment(functionName));
        wasmFunction->Build();

        wasmFunction->SetFunctionName(functionName);

        if (!wasmFunction || !wasmFunction->GetFunctionContext())
        {
            return;
        } 

        m_mpGlobalFunctions[functionName] = wasmFunction;

        return;
    }

    if (!function)
    {
        return;
    }

    if (DoesGlobalFunctionExist(functionName))
    {
        DeleteGlobalFunction(functionName);
    }

    CWebAssemblyApiEnviornment sharedEnv = CreateApiEnvironment(functionName);
    
    struct Env
    {
        CWebAssemblyFunction*      function;
        CWebAssemblyApiEnviornment sharedEnv;
    };

    Env* env = new Env();
    env->function = function;
    env->sharedEnv = sharedEnv;

    CWebAssemblyFunction* wasmFunction = new CWebAssemblyFunction(m_pContextStore->GetStore(), function->GetFunctionType(), Wasm_CallSharedImportedFunction, NULL);
    wasmFunction->Build((void*)env);

    wasmFunction->SetFunctionName(functionName);
    wasmFunction->SetApiEnviornment(sharedEnv);

    if (!wasmFunction || !wasmFunction->GetFunctionContext())
    {
        return;
    }

    m_mpGlobalFunctions[functionName] = wasmFunction;
}

WebAssemblyApi(CWebAssemblyScript::Wasm_CallSharedImportedFunction, env, args, results)
{
    struct Env
    {
        CWebAssemblyFunction*      function;
        CWebAssemblyApiEnviornment sharedEnv;
    };

    Env* enviornment = (Env*)env;

    CWebAssemblyFunction* function = enviornment->function;

    CWebAssemblyArgReader argStream(enviornment->sharedEnv, args, results);

    if (!function || !(*function))
    {
        return argStream.ReturnNull("Couldn't call function.\n");
    }
    
    CWebAssemblyVariables funcArgs, funcResults;

    for (size_t i = 0; i < args->num_elems; i++)
    {
        funcArgs.Push(args->data[i]);
    }

    SString errorMessage = "";

    if (!function->Call(&funcArgs, &funcResults, &errorMessage))
    {
        if (!errorMessage.empty())
        {
            return argStream.ReturnNull(errorMessage);
        }
    }

    if (funcResults.GetSize() < 1)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(funcResults.GetFirst());
}

WebAssemblyApi(CWebAssemblyScript::Wasm_CallLuaImportedFunction, env, args, results)
{
    CWebAssemblyMemoryPointerAddress ptr;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    int32_t argCount = (int32_t)argStream.GetArgumentCount() - 2;

    if (argCount < 0)
    {
        argCount = 0;
    }

    argStream.Skip(argCount);

    bool canWriteResult = argStream.CanContinue();

    if (canWriteResult)
    {
        argStream.ReadPointerAddress(ptr);
        argStream.ReadUInt32(maxSize);
    }

    CLuaArguments luaArguments;

    for (int i = 0; i < argCount; i++)
    {
        CWebAssemblyValue arg = args->data[i];

        switch (arg.kind)
        {
            case C_WASM_VARIABLE_TYPE_I32:
                luaArguments.PushNumber((double)arg.of.i32);
                break;
            case C_WASM_VARIABLE_TYPE_I64:
                luaArguments.PushNumber((double)arg.of.i64);
                break;
            case C_WASM_VARIABLE_TYPE_F32:
                luaArguments.PushNumber((double)arg.of.f32);
                break;
            case C_WASM_VARIABLE_TYPE_F64:
                luaArguments.PushNumber((double)arg.of.f64);
                break;
            default:
                break;
        }
    }

    lua_State* luaVM = argStream.GetScript()->GetStoreContext()->GetResource()->GetVirtualMachine()->GetVM();

    lua_getglobal(luaVM, argStream.GetFunctionName().c_str());

    luaArguments.PushArguments(luaVM);

    if (lua_pcall(luaVM, luaArguments.Count(), canWriteResult ? 1 : 0, 0) != 0)
    {
        SString errorMessage = lua_tostring(luaVM, -1);

        lua_pop(luaVM, 1);

        return argStream.ReturnNull(errorMessage);
    }

    if (canWriteResult)
    {
        int bytesWrote = 0;

        if (ptr != WEB_ASSEMBLY_NULL_PTR)
        {
            int type = lua_type(luaVM, -1);

            if (type == LUA_TBOOLEAN)
            {
                bool result = lua_toboolean(luaVM, -1);

                bytesWrote = std::min((uint32_t)sizeof(result), maxSize);

                argStream.WritePointer(ptr, &result, bytesWrote);
            }
            else if (type == LUA_TNUMBER)
            {
                double result = lua_tonumber(luaVM, -1);

                bytesWrote = std::min((uint32_t)sizeof(result), maxSize);

                argStream.WritePointer(ptr, &result, bytesWrote);
            }
            else if (type == LUA_TSTRING)
            {
                const char* result = lua_tostring(luaVM, -1);

                uint32_t stringSize = lua_strlen(luaVM, -1) + 1;

                bytesWrote = std::min(stringSize, maxSize);

                argStream.WritePointer(ptr, result, bytesWrote);

                char endCharacter = '\0';

                argStream.WritePointer(ptr + stringSize, &endCharacter);
            }
            else if (type == LUA_TUSERDATA || type == LUA_TLIGHTUSERDATA)
            {
                CWebAssemblyUserData result = (CWebAssemblyUserData)lua_touserdata(luaVM, -1);

                bytesWrote = std::min((uint32_t)sizeof(result), maxSize);

                argStream.WritePointer(ptr, &result, bytesWrote);
            }
            else if (type == LUA_TTABLE)
            {    
                bool isFunctionReference = false;

                if (lua_getmetatable(luaVM, -1))
                {
                    if (lua_type(luaVM, -1) == LUA_TTABLE)
                    {
                        lua_getfield(luaVM, -1, "__call");
                        if (lua_type(luaVM, -1) == LUA_TFUNCTION)
                        {
                            lua_CFunction caller = lua_tocfunction(luaVM, -1);
                            if (caller == CLuaArgument::CallFunction)
                            {
                                isFunctionReference = true;
                            }
                        }
                        lua_pop(luaVM, 1);
                    }
                    lua_pop(luaVM, 1);
                }

                if (isFunctionReference)
                {
                    lua_getfield(luaVM, -1, "resource");
                    lua_getfield(luaVM, -2, "reference");

                    if (lua_type(luaVM, -2) == LUA_TLIGHTUSERDATA)
                    {
                        if (lua_type(luaVM, -1) == LUA_TNUMBER)
                        {
                            CCallable callable(lua_toresource(luaVM, -2), lua_tonumber(luaVM, -1));

                            uint8_t hash[C_CALLABLE_HASH_SIZE];

                            callable.WriteHash(hash);

                            bytesWrote = std::min((uint32_t)C_CALLABLE_HASH_SIZE, maxSize);

                            argStream.WritePointer(ptr, hash, bytesWrote);
                        }
                        else if (lua_type(luaVM, -1) == LUA_TLIGHTUSERDATA)
                        {
                            CWebAssemblyFunction* function = (CWebAssemblyFunction*)lua_touserdata(luaVM, -1);

                            if (function)
                            {
                                CCallable callable(function);

                                callable.SetLuaResource(function->GetApiEnviornment()->script->GetStoreContext()->GetResource());
                                callable.SetIsWasmFunctionState(true);

                                uint8_t hash[C_CALLABLE_HASH_SIZE];

                                callable.WriteHash(hash);

                                bytesWrote = std::min((uint32_t)C_CALLABLE_HASH_SIZE, maxSize);

                                argStream.WritePointer(ptr, hash, bytesWrote);
                            }
                        }
                    }
                    lua_pop(luaVM, 2);
                }
            }
            else if (type == LUA_TFUNCTION)
            {
                lua_pushvalue(luaVM, -1);
                int funcRef = luaL_ref(luaVM, LUA_REGISTRYINDEX);

                CLuaMain& luaMain = lua_getownercluamain(luaVM);

                CCallable callable(luaMain.GetResource(), funcRef);

                uint8_t hash[C_CALLABLE_HASH_SIZE];

                callable.WriteHash(hash);

                bytesWrote = std::min((uint32_t)C_CALLABLE_HASH_SIZE, maxSize);

                argStream.WritePointer(ptr, hash, bytesWrote);
            }
        }

        lua_pop(luaVM, 1);

        return argStream.Return(bytesWrote);
    }

    return argStream.ReturnNull();
}

CWebAssemblyContext* CWebAssemblyScript::GetStoreContext()
{
    return m_pContextStore;
}

CWebAssemblyStore* CWebAssemblyScript::GetStore()
{
    return m_pStore;
}

CWebAssemblyModuleContext CWebAssemblyScript::GetModule()
{
    return m_pModule;
}

CWebAssemblyModuleContext CWebAssemblyScript::GetSharedModule()
{
    return m_pSharedModule;
}

CWebAssemblyInstanceContext CWebAssemblyScript::GetInstance()
{
    return m_pInstance;
}

void CWebAssemblyScript::BuildExportedFunctions()
{
    if (m_mpExports.empty())
    {
        return;
    }

    for (const std::pair<SString, CWebAssemblyExtern>& item : m_mpExports)
    {
        SString            externName = item.first;
        CWebAssemblyExtern externData = item.second;

        if (externData.kind != C_WASM_EXTERN_TYPE_FUNCTION)
        {
            continue;
        }

        wasm_externtype_t* externType = wasm_extern_type(externData.context);
        wasm_functype_t*   funcType = wasm_externtype_as_functype(externType);

        CWebAssemblyFunctionContext functionContext = wasm_extern_as_func(externData.context);

        CWebAssemblyFunctionType functionType(funcType);

        CWebAssemblyFunction* function = new CWebAssemblyFunction();

        function->SetStore(m_pContextStore->GetStore());
        function->SetFunctionType(functionType);
        function->SetFunctionContext(functionContext);
        function->SetApiEnviornment(CreateApiEnvironment(externName));
        function->SetFunctionName(externName);

        m_mpExportedFunctions[externName] = function;
    }
}

void CWebAssemblyScript::BuildInternalFunctions()
{
    CWebAssemblyExtern ext = GetExport(WASM_INTERNAL_FUNCTIONS_TABLE_EXPORT_NAME);

    if (!IsExternValid(ext))
    {
        return;
    }

    if (ext.kind != C_WASM_EXTERN_TYPE_TABLE)
    {
        return;
    }

    wasm_table_t* functions = wasm_extern_as_table(ext.context);

    if (!functions)
    {
        return;
    }

    size_t count = wasm_table_size(functions);

    if (count < 1)
    {
        return;
    }

    for (size_t i = 0; i < count; i++)
    {
        wasm_ref_t* funcRef = wasm_table_get(functions, i);

        wasm_func_t* wasmFunc = wasm_ref_as_func(funcRef);

        wasm_functype_t* wasmFuncType = wasm_func_type(wasmFunc);

        CWebAssemblyFunctionType funcType;
        funcType.ReadFunctionTypeContext(wasmFuncType);

        CWebAssemblyFunction* internalFunction = new CWebAssemblyFunction();

        internalFunction->SetStore(m_pContextStore->GetStore());
        internalFunction->SetFunctionType(funcType);
        internalFunction->SetFunctionContext(wasmFunc);
        internalFunction->SetApiEnviornment(CreateApiEnvironment(""));

        m_lsInternalFunctions.push_back(internalFunction);
    }
}

void CWebAssemblyScript::BuildMemory()
{
    if (m_mpExports.empty())
    {
        return;
    }

    for (const std::pair<SString, CWebAssemblyExtern>& item : m_mpExports)
    {
        SString            externName = item.first;
        CWebAssemblyExtern externData = item.second;
            
        if (externData.kind != C_WASM_EXTERN_TYPE_MEMORY)
        {
            continue;
        }

        if (m_pMemory)
        {
            DeleteMemory();
        }

        CWebAssemblyMemoryContext memory = wasm_extern_as_memory(externData.context);

        m_pMemory = new CWebAssemblyMemory(this, memory);

        break;
    }
}

CWebAssemblyExternMap& CWebAssemblyScript::GetExports()
{
    return m_mpExports;
}

CWebAssemblyFunctionMap& CWebAssemblyScript::GetExportedFunctions()
{
    return m_mpExportedFunctions;
}

CWebAssemblyExtern CWebAssemblyScript::GetExport(const SString& exportName)
{
    if (!DoesExportExist(exportName))
    {
        CWebAssemblyExtern dummyExtern;
        dummyExtern.context = NULL;
        dummyExtern.kind = NULL;

        return dummyExtern;
    }

    return m_mpExports[exportName];
}

size_t CWebAssemblyScript::GetInternalFunctionsCount()
{
    return m_lsInternalFunctions.size();
}

CWebAssemblyMemory* CWebAssemblyScript::GetMemory()
{
    return m_pMemory;
}

CWebAssemblyFunction* CWebAssemblyScript::GetExportedFunction(const SString& functionName)
{
    if (!DoesExportedFunctionExist(functionName))
    {
        return NULL;
    }

    return m_mpExportedFunctions[functionName];
}

CWebAssemblyFunctionMap& CWebAssemblyScript::GetApiFunctions()
{
    return m_mpApiFunctions;
}

CWebAssemblyFunctionMap& CWebAssemblyScript::GetGlobalFunctions()
{
    return m_mpGlobalFunctions;
}

bool CWebAssemblyScript::DoesExportExist(const SString& exportName)
{
    return m_mpExports.find(exportName) != m_mpExports.end();
}

bool CWebAssemblyScript::DoesExportedFunctionExist(const SString& functionName)
{
    return m_mpExportedFunctions.find(functionName) != m_mpExportedFunctions.end();
}

bool CWebAssemblyScript::DoesApiFunctionExist(const SString& functionName)
{
    return m_mpApiFunctions.find(functionName) != m_mpApiFunctions.end();
}

bool CWebAssemblyScript::DoesGlobalFunctionExist(const SString& functionName)
{
    return m_mpGlobalFunctions.find(functionName) != m_mpGlobalFunctions.end();
}

CWebAssemblyFunction* CWebAssemblyScript::GetApiFunction(const SString& functionName)
{
    if (m_mpApiFunctions.empty())
    {
        return NULL;
    }

    if (!DoesApiFunctionExist(functionName))
    {
        return NULL;
    }

    return m_mpApiFunctions[functionName];
}

CWebAssemblyFunction* CWebAssemblyScript::GetGlobalFunction(const SString& functionName)
{
    if (m_mpGlobalFunctions.empty())
    {
        return NULL;
    }

    if (!DoesGlobalFunctionExist(functionName))
    {
        return NULL;
    }

    return m_mpGlobalFunctions[functionName];
}

CWebAssemblyFunction* CWebAssemblyScript::GetInternalFunction(const size_t& index)
{
    size_t count = m_lsInternalFunctions.size();

    if (count < 1)
    {
        return NULL;
    }

    if (index >= count)
    {
        return NULL;
    }

    return m_lsInternalFunctions[index];
}

CWebAssemblyFunction* CWebAssemblyScript::GetInternalFunctionByHash(const uintptr_t& hash)
{
    size_t index = GetInternalFunctionIndexByHash(hash);

    if (index < 1)
    {
        return NULL;
    }

    return GetInternalFunction(index);
}

uintptr_t CWebAssemblyScript::GetInternalFunctionHash(const size_t& index)
{
    if (index < 1 || index > GetInternalFunctionsCount())
    {
        return 0;
    }

    return (uintptr_t)(GetMemory()->GetBase()) + (uintptr_t)index;
}

size_t CWebAssemblyScript::GetInternalFunctionIndexByHash(const uintptr_t& hash)
{
    CWebAssemblyMemory* memory = GetMemory();

    if (!memory->DoesPointerBelongToMemory((void*)hash))
    {
        return 0;
    }

    return hash - (uintptr_t)memory->GetBase();
}

bool CWebAssemblyScript::IsValidInternalFunctionHash(const uintptr_t& hash)
{
    size_t index = GetInternalFunctionIndexByHash(hash);

    return GetMemory()->DoesPointerBelongToMemory((void*)hash) && index > 0 && index < GetInternalFunctionsCount();
}

void CWebAssemblyScript::DeleteExportedFunction(const SString& functionName)
{
    if (functionName.empty())
    {
        return;
    }

    if (!DoesExportedFunctionExist(functionName))
    {
        return;
    }

    CWebAssemblyFunction* function = m_mpExportedFunctions[functionName];

    if (!function)
    {
        return;
    }

    m_mpExportedFunctions.erase(functionName);

    delete function;
}

void CWebAssemblyScript::DeleteApiFunction(const SString& functionName)
{
    if (functionName.empty())
    {
        return;
    }

    if (!DoesApiFunctionExist(functionName))
    {
        return;
    }

    CWebAssemblyFunction* function = m_mpApiFunctions[functionName];

    if (!function)
    {
        return;
    }

    m_mpApiFunctions.erase(functionName);

    delete function;
}

void CWebAssemblyScript::DeleteGlobalFunction(const SString& functionName)
{
    if (functionName.empty())
    {
        return;
    }

    if (!DoesGlobalFunctionExist(functionName))
    {
        return;
    }

    CWebAssemblyFunction* function = m_mpGlobalFunctions[functionName];

    if (!function)
    {
        return;
    }

    m_mpGlobalFunctions.erase(functionName);

    delete function;
}

void CWebAssemblyScript::ClearExportedFunctions()
{
    if (m_mpExportedFunctions.empty())
    {
        return;
    }

    for (const std::pair<SString, CWebAssemblyFunction*>& item : m_mpExportedFunctions)
    {
        CWebAssemblyFunction* function = item.second;

        if (!function)
        {
            continue;
        }

        delete function;
    }

    m_mpExportedFunctions.clear();
}

void CWebAssemblyScript::ClearInternalFunctions()
{
    if (m_lsInternalFunctions.empty())
    {
        return;
    }

    size_t count = m_lsInternalFunctions.size();

    for (size_t i = 0; i < count; i++)
    {
        CWebAssemblyFunction* function = m_lsInternalFunctions[i];

        if (!function)
        {
            continue;
        }

        delete function;
    }

    m_lsInternalFunctions.clear();
}

void CWebAssemblyScript::ClearApiFunctions()
{
    if (m_mpApiFunctions.empty())
    {
        return;
    }

    for (const std::pair<SString, CWebAssemblyFunction*>& item : m_mpApiFunctions)
    {
        CWebAssemblyFunction* function = item.second;

        if (!function)
        {
            continue;
        }

        delete function;
    }

    m_mpApiFunctions.clear();
}

void CWebAssemblyScript::ClearGlobalFunctions()
{
    if (m_mpGlobalFunctions.empty())
    {
        return;
    }

    for (std::pair<SString, CWebAssemblyFunction*> item : m_mpGlobalFunctions)
    {
        CWebAssemblyFunction* function = item.second;

        if (!function)
        {
            continue;
        }

        delete function;
    }

    m_mpGlobalFunctions.clear();
}

void CWebAssemblyScript::InsertSharedGlobalFunctions()
{
    if (m_mpExportedFunctions.empty())
    {
        return;
    }

    for (const std::pair<SString, CWebAssemblyFunction*>& item : m_mpExportedFunctions)
    {
        SString               name = item.first;
        CWebAssemblyFunction* function = item.second;

        if (name == WASM_MAIN_FUNCTION_NAME || name == WASM_MALLOC_FUNCTION_NAME || name == WASM_FREE_FUNCTION_NAME)
        {
            continue;
        }

        m_pContextStore->SetGlobalFunction(name, function);
    }
}

void CWebAssemblyScript::ClearSharedGlobalFunctions()
{
    CWebAssemblyFunctionMap& globalFunctions = m_pContextStore->GetGlobalFunctions();

    if (globalFunctions.empty())
    {
        return;
    }

    for (std::pair<SString, CWebAssemblyFunction*> item : globalFunctions)
    {
        CWebAssemblyFunction* function = item.second;

        if (function->GetApiEnviornment()->script == this)
        {
            globalFunctions.erase(item.first);
        }
    }
}

void CWebAssemblyScript::DeleteMemory()
{
    if (!m_pMemory)
    {
        return;
    }

    delete m_pMemory;
    m_pMemory = NULL;
}

SString CWebAssemblyScript::GetScriptFile()
{
    return m_strScriptFile;
}

SString CWebAssemblyScript::GetResourcePath()
{
    SString result = m_pContextStore->GetResource()->GetName();
    result += "\\";
    result += m_strScriptFile;

    return result;
}

CWebAssemblyApiEnviornment CWebAssemblyScript::CreateApiEnvironment(const SString& functionName)
{
    CWebAssemblyApiEnviornment env = new CWebAssemblyApiEnvironmentObject();

    env->script = this;
    env->functionName = functionName;

    return env;
}

CWebAssemblySharedScriptData* CWebAssemblyScript::CreateSharedScriptData(CWebAssemblyStore* store, pthread_t* threadId)
{
    CWebAssemblySharedScriptData* sharedScriptData = (CWebAssemblySharedScriptData*)malloc(sizeof(CWebAssemblySharedScriptData));

    if (!sharedScriptData)
    {
        return NULL;
    }

    memset(sharedScriptData, 0, sizeof(CWebAssemblySharedScriptData));

    sharedScriptData->script = this;
    sharedScriptData->store = store ? store : m_pStore;
    sharedScriptData->threadId = threadId ? *threadId : pthread_self();

    return sharedScriptData;
}

bool CWebAssemblyScript::IsExternValid(const CWebAssemblyExtern& waExtern)
{
    return waExtern.context != NULL;
}

void CWebAssemblyScript::DeleteSharedScriptData(CWebAssemblySharedScriptData* sharedScriptData)
{
    if (!sharedScriptData)
    {
        return;
    }

    free((void*)sharedScriptData);
}

CWebAssemblyMemory::CWebAssemblyMemory()
{
    m_pContext = NULL;
    m_pScript = NULL;
}

CWebAssemblyMemory::CWebAssemblyMemory(CWebAssemblyScript* script)
{
    SetScript(script);
}

CWebAssemblyMemory::CWebAssemblyMemory(CWebAssemblyMemoryContext context)
{
    SetContext(context);
}

CWebAssemblyMemory::CWebAssemblyMemory(CWebAssemblyScript* script, CWebAssemblyMemoryContext context)
{
    SetScript(script);
    SetContext(context);

    byte_t* base = wasm_memory_data(context);
    size_t size = wasm_memory_data_size(context);
}

CWebAssemblyMemory::~CWebAssemblyMemory()
{
    Destroy();
}

void CWebAssemblyMemory::Destroy()
{
    if (!m_pContext)
    {
        return;
    }

    wasm_memory_delete(m_pContext);
    m_pContext = NULL;
}

CWebAssemblyMemoryPointerAddress CWebAssemblyMemory::Malloc(const size_t& size, void** physicalPointer)
{
    if (size < 1)
    {
        return WEB_ASSEMBLY_NULL_PTR;
    }

    CWebAssemblyFunction* moduleMallocFunction = m_pScript->GetExportedFunction(WASM_MALLOC_FUNCTION_NAME);

    if (!moduleMallocFunction)
    {
        CLogger::ErrorPrintf("Couldn't find module `%s` function[\"@%s/%s\"].\n", WASM_MALLOC_FUNCTION_NAME, m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
        return WEB_ASSEMBLY_NULL_PTR;
    }

    CWebAssemblyVariables args;
    CWebAssemblyVariables results;

    args.Push((int32_t)size);

    if (!moduleMallocFunction->Call(&args, &results))
    {
        CLogger::ErrorPrintf("Cloudn't allocate memory blocks for module[\"@%s/%s\"].\n", m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
        return WEB_ASSEMBLY_NULL_PTR;
    }

    if (results.GetSize() < 1)
    {
        return WEB_ASSEMBLY_NULL_PTR;
    }

    CWebAssemblyMemoryPointerAddress pointerAddress = results.GetFirst().GetInt32();

    if (physicalPointer)
    {
        *physicalPointer = GetMemoryPhysicalPointer(pointerAddress);
    }

    return pointerAddress;
}


CWebAssemblyMemoryPointerAddress CWebAssemblyMemory::Realloc(CWebAssemblyMemoryPointerAddress pointer, const size_t& size, void** physicalPointer)
{
    CWebAssemblyFunction* moduleReallocFunction = m_pScript->GetExportedFunction(WASM_REALLOC_FUNCTION_NAME);

    if (!moduleReallocFunction)
    {
        CLogger::ErrorPrintf("Couldn't find module `%s` function[\"@%s/%s\"].\n", WASM_REALLOC_FUNCTION_NAME, m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
        return WEB_ASSEMBLY_NULL_PTR;
    }

    CWebAssemblyVariables args, results;

    args.Push((int32_t)pointer);
    args.Push((int32_t)size);
    
    if (!moduleReallocFunction->Call(&args, &results))
    {
        CLogger::ErrorPrintf("Cloudn't re-allocate memory blocks for module[\"@%s/%s\"].\n", m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
        return WEB_ASSEMBLY_NULL_PTR;
    }

    if (results.GetSize() < 1)
    {
        return WEB_ASSEMBLY_NULL_PTR;
    }

    CWebAssemblyMemoryPointerAddress pointerAddress = results.GetFirst().GetInt32();

    if (physicalPointer) {
        *physicalPointer = GetMemoryPhysicalPointer(pointerAddress);
    }

    return pointerAddress;
}

void CWebAssemblyMemory::Free(CWebAssemblyMemoryPointerAddress pointer)
{
    if (pointer == WEB_ASSEMBLY_NULL_PTR)
    {
        return;
    }
    
    CWebAssemblyFunction* moduleFreeFunction = m_pScript->GetExportedFunction(WASM_FREE_FUNCTION_NAME);

    if (!moduleFreeFunction)
    {
        CLogger::ErrorPrintf("Couldn't find module `%s` function[\"@%s/%s\"].\n", WASM_FREE_FUNCTION_NAME, m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
        return;
    }

    CWebAssemblyVariables args;
    
    args.Push((int32_t)pointer);

    if (!moduleFreeFunction->Call(&args, NULL))
    {
        CLogger::ErrorPrintf("Cloudn't free memory blocks for module[\"@%s/%s\"].\n", m_pScript->GetStoreContext()->GetResource()->GetName().c_str(), m_pScript->GetScriptFile().c_str());
    }
}

CWebAssemblyMemoryPointerAddress CWebAssemblyMemory::StringToUTF8(const SString& str)
{
    size_t length = str.length();

    if (length < 1)
    {
        return WEB_ASSEMBLY_NULL_PTR;
    }

    size_t size = length + 1;

    void* strPtr = NULL;
    CWebAssemblyMemoryPointerAddress pointer = Malloc(size, &strPtr);

    memcpy(strPtr, (void*)str.data(), length);

    ((char*)strPtr)[length] = '\0';

    return pointer;
}

SString CWebAssemblyMemory::UTF8ToString(CWebAssemblyMemoryPointerAddress pointer, intptr_t size)
{
    SString result = "";

    if (pointer == WEB_ASSEMBLY_NULL_PTR)
    {
        return result;
    }

    char* string = (char*)GetMemoryPhysicalPointer(pointer);

    size_t length = size == -1 ? strlen(string) : size;

    if (length < 1)
    {
        return result;
    }

    result = std::string(string, length);

    return result;
}

void* CWebAssemblyMemory::GetBase()
{
    if (!m_pContext)
    {
        return NULL;
    }

    return wasm_memory_data(m_pContext);
}

size_t CWebAssemblyMemory::GetSize()
{
    if (!m_pContext)
    {
        return 0;
    }

    return wasm_memory_data_size(m_pContext);
}

uintptr_t CWebAssemblyMemory::GetMemoryPhysicalPointerAddress(CWebAssemblyMemoryPointerAddress pointer)
{
    if (pointer == WEB_ASSEMBLY_NULL_PTR)
    {
        return 0;
    }

    return ((uintptr_t)GetBase()) + pointer;
}

void* CWebAssemblyMemory::GetMemoryPhysicalPointer(CWebAssemblyMemoryPointerAddress pointer)
{
    if (pointer == WEB_ASSEMBLY_NULL_PTR)
    {
        return NULL;
    }

    return (void*)((byte_t*)GetBase() + pointer);
}

CWebAssemblyMemoryPointerAddress CWebAssemblyMemory::GetMemoryPointerAddressByPhysicalAddress(void* pointer)
{
    if (!DoesPointerBelongToMemory(pointer))
    {
        return WEB_ASSEMBLY_NULL_PTR;
    }

    return ((uintptr_t)pointer) - ((uintptr_t)GetBase());
}

void CWebAssemblyMemory::SetScript(CWebAssemblyScript* script)
{
    m_pScript = script;
}

CWebAssemblyScript* CWebAssemblyMemory::GetScript()
{
    return m_pScript;
}

void CWebAssemblyMemory::SetContext(CWebAssemblyMemoryContext context)
{
    m_pContext = context;
}

CWebAssemblyMemoryContext CWebAssemblyMemory::GetContext()
{
    return m_pContext;
}

size_t CWebAssemblyMemory::GetFreeSpaceSize()
{
    #define DIGIT_COUNT(n) (std::floor(std::log10(std::abs((double)n) + 1)) + 1)

    size_t memTotalSizeDigitCount = DIGIT_COUNT(GetSize());
    size_t chunksArraySize = (memTotalSizeDigitCount * sizeof(CWebAssemblyMemoryPointerAddress)) * 2;

    CWebAssemblyMemoryPointerAddress* chunks = (CWebAssemblyMemoryPointerAddress*)malloc(chunksArraySize);
    
    if (!chunks)
    {
        return 0;
    }

    memset((void*)chunks, WEB_ASSEMBLY_NULL_PTR, chunksArraySize);

    size_t eachChunkSize = std::pow(10, memTotalSizeDigitCount - 1);
    size_t freeSpace = 0;

    for (size_t i = 0; i < chunksArraySize; i++)
    {
        chunks[i] = Malloc(eachChunkSize);

        if (chunks[i] == WEB_ASSEMBLY_NULL_PTR)
        {
            size_t remainedSize = eachChunkSize;
            size_t remainedSizeDigitCount = DIGIT_COUNT(remainedSize);

            while (remainedSizeDigitCount > 0)
            {
                #define REMAINED_VALUE ((int)std::pow((double)10, (double)(remainedSizeDigitCount - 1)))

                chunks[i] = Malloc(remainedSize);

                if (chunks[i] != WEB_ASSEMBLY_NULL_PTR || remainedSize == 0)
                {
                    Free(chunks[i]);
                    chunks[i] = WEB_ASSEMBLY_NULL_PTR;

                    remainedSize += REMAINED_VALUE;
                    remainedSizeDigitCount -= 1;

                    if (remainedSizeDigitCount > 0)
                    {
                        remainedSize -= REMAINED_VALUE;
                    }

                    continue;
                }

                remainedSize -= REMAINED_VALUE;

                #undef REMAINED_VALUE
            }

            freeSpace += (i * eachChunkSize) + remainedSize;

            break;
        }
    }

    for (size_t i = 0; i < memTotalSizeDigitCount; i++)
    {
        if (chunks[i] != WEB_ASSEMBLY_NULL_PTR)
        {
            Free(chunks[i]);
        }
    }

    free((void*)chunks);

    #undef DIGIT_COUNT

    return freeSpace;
}

bool CWebAssemblyMemory::DoesPointerBelongToMemory(void* ptr)
{
    if (!ptr)
    {
        return false;
    }

    intptr_t address = (intptr_t)ptr;
    intptr_t base = (intptr_t)GetBase();

    return address >= base && address <= (base + GetSize());
}
