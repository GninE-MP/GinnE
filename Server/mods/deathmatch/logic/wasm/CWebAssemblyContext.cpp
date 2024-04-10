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

CWebAssemblyEngine* WebAssemblyEngine = NULL;

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
    m_pContext = wasm_engine_new();
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

CWebAssemblyContext::CWebAssemblyContext()
{
    m_pModuleBinary = NULL;
    m_uiModuleBinarySize = 0;

    m_bLoaded = false;

    m_pStore = NULL;

    m_lsImports.clear();
}

CWebAssemblyContext::~CWebAssemblyContext()
{
    Unload();

    FreeModuleBinary();

    ClearImports();
}

CWebAssemblyLoadState CWebAssemblyContext::LoadBinary(const char* binary, const size_t& binarySize)
{
    SetModuleBinarySize(binarySize);
    SetModuleBinary(binary);

    return Load();
}

wasm_trap_t* mta_print(const wasm_val_vec_t* args, wasm_val_vec_t* results)
{
    CLogger::LogPrintf("this is mta print function honey!\n");
    return NULL;
}

CWebAssemblyLoadState CWebAssemblyContext::Load()
{
    if (m_bLoaded)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (!m_pModuleBinary)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (m_uiModuleBinarySize < 1)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (!IsWebAssemblyBinary(m_pModuleBinary))
    {
        return CWebAssemblyLoadState::Failed;
    }

    m_pStore = new CWebAssemblyStore(WebAssemblyEngine);
    m_pStore->Build();

    wasm_byte_vec_t binary;
    wasm_byte_vec_new_uninitialized(&binary, m_uiModuleBinarySize);
    
    memcpy(binary.data, m_pModuleBinary, m_uiModuleBinarySize);

    if (!wasm_module_validate(m_pStore->GetContext(), &binary))
    {
        /*m_waEngine.Destroy();
        m_waStore.Destroy();

        wasm_byte_vec_delete(&binary);

        return CWebAssemblyLoadState::Failed;*/

        goto Fail;
    }

    CWebAssemblyModuleContext wModule = wasm_module_new(m_pStore->GetContext(), &binary);

    wasm_byte_vec_delete(&binary);

    if (!wModule)
    {
        goto Fail;
    }

    
    /*wasm_exporttype_vec_t exports;
    wasm_module_exports(wModule, &exports);

    for (int i = 0; i < exports.num_elems; i++)
    {
        wasm_exporttype_t* data = exports.data[i];

        const wasm_name_t* nameD = wasm_exporttype_name(data);

        const wasm_externtype_t* externT = wasm_exporttype_type(data);

        wasm_externkind_t kind = wasm_externtype_kind(externT);

        CLogger::LogPrintf("export is : [%s], %d\n", nameD->data, kind);
    }

    wasm_functype_t* print_type = wasm_functype_new_0_0();
    wasm_func_t*     print_func = wasm_func_new(m_pStore->GetContext(), print_type, mta_print);

    wasm_functype_delete(print_type);

    wasm_extern_t*    importExterns[] = { wasm_func_as_extern(print_func) };
    wasm_extern_vec_t imports = WASM_ARRAY_VEC(importExterns);*/ 

    wasm_trap_t* trap = NULL;

    CWebAssemblyInstanceContext wInstance = wasm_instance_new(m_pStore->GetContext(), wModule, NULL, &trap);

    //wasm_func_delete(print_func);

    CLogger::LogPrintf("instance = %d, module = %d, store = %d\n", wInstance, wModule, m_pStore->GetContext());

    if (trap || !wInstance)
    {
        goto Fail;
    }

    trap = NULL;



    m_bLoaded = true;
    
    return CWebAssemblyLoadState::Succeed;

    Fail:
    {
        if (wInstance)
        {
            wasm_instance_delete(wInstance);
        }

        if (wModule)
        {
            wasm_module_delete(wModule);
        }

        if (binary.data)
        {
            wasm_byte_vec_delete(&binary);
        }

        if (trap)
        {
            wasm_message_t message;
            wasm_trap_message(trap, &message);

            CLogger::ErrorPrintf("we have trap! : [%s]\n", message.data);

            wasm_trap_delete(trap);
        }

        m_pStore->Destroy();

        delete m_pStore;
        m_pStore = NULL;

        return CWebAssemblyLoadState::Failed;
    }
}

void CWebAssemblyContext::Unload()
{
    if (!m_bLoaded)
    {
        return;
    }

    m_bLoaded = false;
}

void CWebAssemblyContext::Import(const CWebAssemblyCFunction& function)
{
    if (!function)
    {
        return;
    }


}

void CWebAssemblyContext::ClearImports()
{
    m_lsImports.clear();
}

CWebAssemblyImports& CWebAssemblyContext::GetImports()
{
    return m_lsImports;
}

void CWebAssemblyContext::SetModuleBinary(const char* binary)
{
    if (m_uiModuleBinarySize < 1)
    {
        return;
    }

    if (m_pModuleBinary)
    {
        FreeModuleBinary();
    }

    m_pModuleBinary = (char*)malloc(m_uiModuleBinarySize);

    if (!m_pModuleBinary)
    {
        delete m_pModuleBinary;
        return;
    }

    memcpy(m_pModuleBinary, (char*)binary, m_uiModuleBinarySize);
}

const char* CWebAssemblyContext::GetModuleBinary()
{
    return m_pModuleBinary;
}

void CWebAssemblyContext::SetModuleBinarySize(const size_t& binarySize)
{
    m_uiModuleBinarySize = binarySize;
}

size_t CWebAssemblyContext::GetModuleBinarySize()
{
    return m_uiModuleBinarySize;
}

void CWebAssemblyContext::FreeModuleBinary()
{
    if (m_pModuleBinary)
    {
        free(m_pModuleBinary);
    }

    m_pModuleBinary = NULL;
    m_uiModuleBinarySize = 0;
}

CWebAssemblyStore* CWebAssemblyContext::GetStore()
{
    return m_pStore;
}

void CWebAssemblyContext::ReleaseBinary()
{
    if (!m_pModuleBinary)
    {
        return;
    }

    free(m_pModuleBinary);
}

bool CWebAssemblyContext::IsLoaded()
{
    return m_bLoaded;
}

void CWebAssemblyContext::InitializeWebAssemblyEngine()
{
    if (WebAssemblyEngine)
    {
        DeleteWebAssemblyEngine();
    }

    CLogger::LogPrintf("Building new web assembly engine...\n");

    WebAssemblyEngine = new CWebAssemblyEngine();
    WebAssemblyEngine->Build();

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
}

CWebAssemblyEngine* CWebAssemblyContext::GetWebAssemblyEngine()
{
    return WebAssemblyEngine;
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
