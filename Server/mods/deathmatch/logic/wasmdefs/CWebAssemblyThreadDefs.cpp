/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyThreadDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CStaticFunctionDefinitions.h"
#include "CDummy.h"
#include "Utils.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyThreadDefs.h"

#include <pthread.h>

void CWebAssemblyThreadDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_worker", "u**");
    SetFunctionType("terminate_worker", "bu");
    SetFunctionType("run_worker", "bu");
    SetFunctionType("worker_join", "bu");
    SetFunctionType("get_current_worker", "u");
    SetFunctionType("get_main_worker", "u");
    SetFunctionType("sleep_worker", "vi");
    SetFunctionType("get_worker_state", "iu");
    SetFunctionType("is_worker", "bu");

    SetFunctionType("create_mutex", "u");
    SetFunctionType("destroy_mutex", "bu");
    SetFunctionType("lock_mutex", "bu");
    SetFunctionType("unlock_mutex", "bu");
    SetFunctionType("is_mutex", "bu");
}

void CWebAssemblyThreadDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_worker", CreateWorker },
        { "terminate_worker", TerminateWorker },
        { "run_worker", RunWorker },
        { "worker_join", JoinWorker },
        { "get_current_worker", GetCurrentWorker },
        { "get_main_worker", GetMainWorker },
        { "sleep_worker", SleepWorker },
        { "get_worker_state", GetWorkerState },
        { "is_worker", IsWorker },

        { "create_mutex", CreateMutex_ },
        { "destroy_mutex", DestroyMutex_ },
        { "lock_mutex", LockMutex_ },
        { "unlock_mutex", UnlockMutex_ },
        { "is_mutex", IsMutex_ }

        //{ "create_thread", CreateThread },
        //{ "terminate_thread", TerminateThread }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyThreadDefs::CreateWorker, env, args, results)
{
    uint32_t                         functionIndex;
    CWebAssemblyMemoryPointerAddress dataPtr;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUInt32(functionIndex);
    argStream.ReadPointerAddress(dataPtr);

    CWebAssemblyScript* script = GetWebAssemblyEnvScript(env);

    if (!script->GetInternalFunction(functionIndex))
    {
        return argStream.ReturnNull();
    }

    CWebAssemblyThreadData threadData;
    memset(&threadData, 0, sizeof(threadData));

    threadData.mainScript = script;
    threadData.functionIndex = functionIndex;
    threadData.data = script->GetMemory()->GetMemoryPhysicalPointer(dataPtr);

    CWebAssemblyThread* worker = GetWebAssemblyEnvContext(env)->CreateThread(threadData);

    if (!worker)
    {
        return argStream.ReturnNull();
    }

    threadData.thread = worker;

    worker->SetData(threadData);

    return argStream.ReturnSystemPointer(worker);
}

WebAssemblyApi(CWebAssemblyThreadDefs::TerminateWorker, env, args, results)
{
    CWebAssemblyThread* worker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(worker);

    if (!worker)
    {
        return argStream.Return(false);
    }

    GetWebAssemblyEnvContext(env)->DestroyThread(worker);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyThreadDefs::RunWorker, env, args, results)
{
    CWebAssemblyThread* worker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(worker);

    if (!worker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(worker->Start());
}

WebAssemblyApi(CWebAssemblyThreadDefs::JoinWorker, env, args, results)
{
    CWebAssemblyThread* worker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(worker);

    if (!worker)
    {
        return argStream.Return(false);
    }

    return argStream.Return(worker->Join());
}

WebAssemblyApi(CWebAssemblyThreadDefs::GetCurrentWorker, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    pthread_t threadId = pthread_self();

    CWebAssemblyThread* mainThread = CWebAssemblyContext::GetMainThread();

    if (threadId.p == mainThread->GetId().p)
    {
        return argStream.ReturnSystemPointer(mainThread);
    }

    CWebAssemblyThread* thread = GetWebAssemblyEnvContext(env)->FindThreadByThreadId(threadId);

    if (!thread)
    {
        return argStream.ReturnNull();
    }

    return argStream.ReturnSystemPointer(thread);
}

WebAssemblyApi(CWebAssemblyThreadDefs::GetMainWorker, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnSystemPointer(CWebAssemblyContext::GetMainThread());
}

WebAssemblyApi(CWebAssemblyThreadDefs::SleepWorker, env, args, results)
{
    uint32_t milliSeconds;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUInt32(milliSeconds);

    pthread_t threadId = pthread_self();

    CWebAssemblyThread* mainThread = CWebAssemblyContext::GetMainThread();

    if (mainThread)
    {
        if (threadId.p == mainThread->GetId().p)
        {
            mainThread->SleepFor(milliSeconds);
            return argStream.ReturnNull();
        }
    }

    CWebAssemblyThread* thread = GetWebAssemblyEnvContext(env)->FindThreadByThreadId(threadId);

    if (!thread)
    {
        return argStream.ReturnNull();
    }

    thread->SleepFor(milliSeconds);

    return argStream.ReturnNull();
}

WebAssemblyApi(CWebAssemblyThreadDefs::GetWorkerState, env, args, results)
{
    CWebAssemblyThread* worker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(worker);

    if (!worker)
    {
        return argStream.Return((int32_t)CWebAssemblyThreadState::Off);
    }

    return argStream.Return((int32_t)worker->GetState());
}

WebAssemblyApi(CWebAssemblyThreadDefs::IsWorker, env, args, results)
{
    CWebAssemblyThread* worker;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(worker);

    if (!worker)
    {
        return argStream.Return(false);
    }

    if (worker == CWebAssemblyContext::GetMainThread())
    {
        return argStream.Return(true);
    }

    return argStream.Return(GetWebAssemblyEnvContext(env)->DoesOwnThread(worker));
}

WebAssemblyApi(CWebAssemblyThreadDefs::CreateMutex_, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.ReturnSystemPointer(GetWebAssemblyEnvContext(env)->CreateThreadMutex());
}

WebAssemblyApi(CWebAssemblyThreadDefs::DestroyMutex_, env, args, results)
{
    CWebAssemblyThreadMutex* mutex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(mutex);

    if (!mutex)
    {
        return argStream.Return(false);
    }

    return argStream.Return(GetWebAssemblyEnvContext(env)->DestroyThreadMutex(mutex));
}

WebAssemblyApi(CWebAssemblyThreadDefs::LockMutex_, env, args, results)
{
    CWebAssemblyThreadMutex* mutex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(mutex);

    if (!mutex)
    {
        return argStream.Return(false);
    }

    return argStream.Return(GetWebAssemblyEnvContext(env)->LockMutex(mutex));
}

WebAssemblyApi(CWebAssemblyThreadDefs::UnlockMutex_, env, args, results)
{
    CWebAssemblyThreadMutex* mutex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(mutex);

    if (!mutex)
    {
        return argStream.Return(false);
    }

    return argStream.Return(GetWebAssemblyEnvContext(env)->UnlockMutex(mutex));
}

WebAssemblyApi(CWebAssemblyThreadDefs::IsMutex_, env, args, results)
{
    CWebAssemblyThreadMutex* mutex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(mutex);

    if (!mutex)
    {
        return argStream.Return(false);
    }

    return argStream.Return(GetWebAssemblyEnvContext(env)->DoesOwnThreadMutex(mutex));
}

struct ThreadArg
{
    CLuaFunctionRef function;
    CLuaArguments   arguments;
    CLuaMain*       luaMain;
    pthread_t*      thread;
};

void* ThreadExecutor(void* arg)
{
    if (!arg)
    {
        pthread_exit(NULL);
        return NULL;
    }

    ThreadArg* threadArg = (ThreadArg*)arg;

    int result;

    result = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (result != 0)
    {
        pthread_exit(NULL);
        return NULL;
    }

    result = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if (result != 0)
    {
        pthread_exit(NULL);
        return NULL;
    }

    pthread_exit(NULL);

    return NULL;
}

WebAssemblyApi(CWebAssemblyThreadDefs::CreateThread, env, args, results)
{
    uint32_t functionIndex;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUInt32(functionIndex);

    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));

    if (!thread)
    {
        return argStream.ReturnNull();
    }

    /*ThreadArg* threadArg = (ThreadArg*)malloc(sizeof(ThreadArg));

    if (!threadArg)
    {
        free((void*)thread);
        return argStream.ReturnNull();
    }

    memcpy(&(threadArg->function), &function, sizeof(function));
    memcpy(&(threadArg->arguments), &arguments, sizeof(arguments));

    threadArg->luaMain = GetWebAssemblyLuaMain(env);
    threadArg->thread = thread;

    CLogger::LogPrintf("sending data to thread: %d\n", threadArg);*/

    {
        CWebAssemblyContext* context = GetWebAssemblyEnvContext(env);

        CWebAssemblyScript* script = GetWebAssemblyEnvScript(env);

        CWebAssemblySharedScriptData* sharedScriptData = script->CreateSharedScriptData();

        CWebAssemblyScript* clonedScript = context->CreateScript();

        CWebAssemblyDefs::RegisterApi(clonedScript);

        CWebAssemblyLoadState loadState = clonedScript->LoadBinary(NULL, 0, script->GetScriptFile(), sharedScriptData);

        CLogger::LogPrintf("could load script successfully : %d, %d\n", clonedScript->GetMemory()->GetBase(), script->GetMemory()->GetBase());
        CLogger::LogPrintf("trying to call function : %d\n", functionIndex);        

        clonedScript->CallInternalFunction(functionIndex, NULL, NULL);

        CWebAssemblyScript::DeleteSharedScriptData(sharedScriptData);
        delete clonedScript;
    }

    //int32_t result = pthread_create(thread, NULL, ThreadExecutor, (void*)threadArg);
    int32_t result = pthread_create(thread, NULL, ThreadExecutor, NULL);

    if (result != 0)
    {
        //free((void*)threadArg);
        free((void*)thread);

        return argStream.ReturnNull();
    }

    return argStream.ReturnSystemPointer(thread);
}

WebAssemblyApi(CWebAssemblyThreadDefs::TerminateThread, env, args, results)
{
    pthread_t* thread;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(thread);

    if (!thread)
    {
        return argStream.Return(false);
    }

    if (pthread_cancel(*thread) != 0)
    {
        return argStream.Return(false);
    }

    // free((void*)thread);

    return argStream.Return(true);
}
