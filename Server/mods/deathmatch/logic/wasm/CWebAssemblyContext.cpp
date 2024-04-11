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

#define WASM_MAIN_FUNCTION_NAME "main"

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
    ClearScripts();

    if (m_pStore)
    {
        delete m_pStore;
    }

    m_pStore = NULL;
}

CWebAssemblyLoadState CWebAssemblyContext::LoadScriptBinary(const char* binary, const size_t& binarySize)
{
    CWebAssemblyScript* script = new CWebAssemblyScript(this);

    CWebAssemblyLoadState state = script->LoadBinary(binary, binarySize);

    if (state == CWebAssemblyLoadState::Succeed)
    {
        m_lsScripts.push_back(script);

        script->CallMainFunction();
    }

    return state;
}

CWebAssemblyScriptList& CWebAssemblyContext::GetScripts()
{
    return m_lsScripts;
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

void CWebAssemblyContext::SetResource(CResource* resource)
{
    m_pResource = resource;
}

CResource* CWebAssemblyContext::GetResource()
{
    return m_pResource;
}

CWebAssemblyStore* CWebAssemblyContext::GetStore()
{
    return m_pStore;
}

/*CWebAssemblyLoadState CWebAssemblyContext::LoadBinary(const char* binary, const size_t& binarySize)
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
    {*/
        /*m_waEngine.Destroy();
        m_waStore.Destroy();

        wasm_byte_vec_delete(&binary);

        return CWebAssemblyLoadState::Failed;*/

        /*goto Fail;
    }

    CWebAssemblyModuleContext wModule = wasm_module_new(m_pStore->GetContext(), &binary);

    wasm_byte_vec_delete(&binary);

    if (!wModule)
    {
        goto Fail;
    }*/ 

    
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

    /*wasm_trap_t* trap = NULL;

    CWebAssemblyInstanceContext wInstance = wasm_instance_new_with_args(m_pStore->GetContext(), wModule, NULL, &trap, 1 * 1024, 1 * 1024);

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
}*/ 

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

CWebAssemblyScript::CWebAssemblyScript()
{
    m_pContextStore = NULL;

    m_pModule = NULL;
    m_pInstance = NULL;
}

CWebAssemblyScript::CWebAssemblyScript(CWebAssemblyContext* context)
{
    m_pContextStore = context;

    m_pModule = NULL;
    m_pInstance = NULL;
}

CWebAssemblyScript::~CWebAssemblyScript()
{
    Destroy();
}

void CWebAssemblyScript::CallMainFunction(int32_t argc, char** argv)
{
    CWebAssemblyExtern mainExtern = GetExport(WASM_MAIN_FUNCTION_NAME);

    if (!IsExternValid(mainExtern))
    {
        return;
    }

    if (mainExtern.kind != C_WASM_EXTERN_TYPE_FUNCTION)
    {
        return;
    }

    wasm_func_t* mainFunction = wasm_extern_as_func(mainExtern.context);

    if (!mainFunction)
    {
        return;
    }

    wasm_val_t AArgc;
    AArgc.kind = WASM_I32;
    AArgc.of.i32 = 0;

    const size_t argsSize = 2;
    wasm_val_t   argsVal[argsSize] = {
        AArgc,
        AArgc
    };

    wasm_val_vec_t argsVec;
    wasm_val_vec_t resultsVec;

    wasm_val_vec_new(&argsVec, argsSize, argsVal);

    //CWebAssemblyTrap* trap = NULL;
    CWebAssemblyTrap* trap = wasm_func_call(mainFunction, &argsVec, &resultsVec);

    CLogger::LogPrintf("called main function : %d and resultsvec is : %d\n", mainFunction, resultsVec.data[0].of.i32);

    if (trap)
    {
        CLogger::ErrorPrintf("Couldn't call module main function.\n");

        //wasm_trap_delete(trap);

        return;
    }


}

void CWebAssemblyScript::Destroy()
{
    if (m_pInstance)
    {
        wasm_instance_delete(m_pInstance);
    }

    if (m_pModule)
    {
        wasm_module_delete(m_pModule);
    }

    m_mpExports.clear();
}

wasm_trap_t* mta_print(const wasm_val_vec_t* args, wasm_val_vec_t* results)
{
    CLogger::LogPrintf("this is mta print function honey!\n");
    return NULL;
}

CWebAssemblyLoadState CWebAssemblyScript::LoadBinary(const char* binary, const size_t& binarySize)
{
    if (binarySize < 1)
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (!CWebAssemblyContext::IsWebAssemblyBinary(binary))
    {
        return CWebAssemblyLoadState::Failed;
    }

    if (!m_pContextStore)
    {
        return CWebAssemblyLoadState::Failed;
    }

    CWebAssemblyStoreContext store = m_pContextStore->GetStore()->GetContext();

    if (!store)
    {
        return CWebAssemblyLoadState::Failed;
    }

    std::vector<SString>                   exportNames;
    std::vector<wasm_externkind_t>         exportKinds;
    std::vector<CWebAssemblyExternContext> exports;

    wasm_byte_vec_t binaryBuffer;
    binaryBuffer.data = NULL;

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

    CWebAssemblyTrap* trap = NULL;

    wasm_functype_t* print_type = wasm_functype_new_0_0();
    wasm_func_t*     print_func = wasm_func_new(store, print_type, mta_print);

    wasm_functype_delete(print_type);

    wasm_extern_t*    importExterns[] = { wasm_func_as_extern(print_func) };
    wasm_extern_vec_t moduleImports = WASM_ARRAY_VEC(importExterns);

    m_pInstance = wasm_instance_new(store, m_pModule, &moduleImports, &trap);

    wasm_func_delete(print_func);

    if (!m_pInstance)
    {
        goto Fail;
    }

    wasm_exporttype_vec_t exportTypes;
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

    wasm_extern_vec_t instanceExports;
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

    return CWebAssemblyLoadState::Succeed;

Fail:
    if (trap)
    {
        wasm_message_t message;
        wasm_trap_message(trap, &message);

        CLogger::ErrorPrintf("Creating new wasm module instance failed : %s\n", message.data);

        wasm_trap_delete(trap);
    }

    if (binaryBuffer.data)
    {
        wasm_byte_vec_delete(&binaryBuffer);
    }

    Destroy();

    return CWebAssemblyLoadState::Failed;
}

CWebAssemblyModuleContext CWebAssemblyScript::GetModule()
{
    return m_pModule;
}

CWebAssemblyInstanceContext CWebAssemblyScript::GetInstance()
{
    return m_pInstance;
}

CWebAssemblyExternMap& CWebAssemblyScript::GetExports()
{
    return m_mpExports;
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

bool CWebAssemblyScript::DoesExportExist(const SString& exportName)
{
    return m_mpExports.find(exportName) != m_mpExports.end();
}

bool CWebAssemblyScript::IsExternValid(const CWebAssemblyExtern& waExtern)
{
    return waExtern.context != NULL;
}
