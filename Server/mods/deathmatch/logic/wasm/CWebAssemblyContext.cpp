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

#define WASM_MAIN_FUNCTION_NAME "ModuleMain"

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

CWebAssemblyScript* CWebAssemblyContext::CreateScript()
{
    return new CWebAssemblyScript(this);
} 

CWebAssemblyLoadState CWebAssemblyContext::LoadScriptBinary(CWebAssemblyScript* script, const char* binary, const size_t& binarySize, const SString& fileName)
{
    if (!script)
    {
        return CWebAssemblyLoadState::Failed;
    }

    CWebAssemblyLoadState state = script->LoadBinary(binary, binarySize, fileName);

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

    /*CWebAssemblyFunction mainFunc = mainFunction;

    CWebAssemblyVariables args, results;

    args.Push(argc);
    args.Push((int32_t)argv);

    CWebAssemblyTrap* trap = mainFunc.Call(&args, &results);*/ 

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
    ClearApiFunctions();
    ClearGlobalFunctions();

    m_mpExports.clear();

    if (m_pInstance)
    {
        wasm_instance_delete(m_pInstance);
    }

    if (m_pModule)
    {
        wasm_module_delete(m_pModule);
    }
}

/*wasm_trap_t* mta_print(const wasm_val_vec_t* args, wasm_val_vec_t* results)
{
    double x = args->data[0].of.f64;

    CLogger::LogPrintf("this is mta print function honey! and result is : %d, %.6f\n", 2.5, 3.14f);

    return NULL;
}*/
/*
//CWebAssemblyTrap* mta_print(void* env, const CWebAssemblyValueVector* args, CWebAssemblyValueVector* results)
WebAssemblyApi(mta_print, env, args, results)
{
    double x = args->data[0].of.f64;

    //CWebAssemblyApiEnviornment* apiEnv = (CWebAssemblyApiEnviornment*)env;

    //CWebAssemblyContext* context = (CWebAssemblyContext*)env;

    CResource* resource = GetWebAssemblyResource(env);

    SString name = resource->GetName();

    CLogger::LogPrintf("data is : %d, resource is : %s\n", (int)x, name.c_str());

    results->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
    results->data[0].of.i32 = (int)x + 1030;

    return NULL;
}
*/
CWebAssemblyLoadState CWebAssemblyScript::LoadBinary(const char* binary, const size_t& binarySize, const SString& fileName)
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

    m_strScriptFile = fileName;

    CWebAssemblyStoreContext store = m_pContextStore->GetStore()->GetContext();

    if (!store)
    {
        return CWebAssemblyLoadState::Failed;
    }

    std::vector<SString>                   exportNames;
    std::vector<wasm_externkind_t>         exportKinds;
    std::vector<CWebAssemblyExternContext> exports;

    CWebAssemblyExternContext* imports = NULL;

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

    wasm_importtype_vec_t moduleImportTypes;
    wasm_module_imports(m_pModule, &moduleImportTypes);

    size_t moduleImportTypesLength = moduleImportTypes.num_elems;
    size_t moduleValidImportsLength = 0;

    if (moduleImportTypesLength > 0)
    {
        for (size_t i = 0; i < moduleImportTypesLength; i++)
        {
            wasm_importtype_t* importType = moduleImportTypes.data[i];

            SString moduleName = wasm_importtype_name(importType)->data;

            if (moduleName == WEB_ASSEMBLY_API_MODULE_NAME)
            {
                moduleValidImportsLength++;
            }
        }

        imports = new CWebAssemblyExternContext[moduleValidImportsLength];

        for (size_t i = 0; i < moduleImportTypesLength; i++)
        {
            wasm_importtype_t* importType = moduleImportTypes.data[i];

            SString moduleName = wasm_importtype_module(importType)->data;
            SString importName = wasm_importtype_name(importType)->data;

            if (moduleName == WEB_ASSEMBLY_API_MODULE_NAME)
            {
                CWebAssemblyFunctionType apiFunctionType;
                CWebAssemblyFunctionType importFuncType;

                
                CWebAssemblyVariables args;
                args.Push(313);

                if (!DoesApiFunctionExist(importName))
                {
                    if (!DoesGlobalFunctionExist(importName))
                    {
                        goto ImportFail;
                    }

                    goto CheckGlobalFunctions;
                }

                CWebAssemblyFunction* apiFunction = m_mpApiFunctions[importName];

                apiFunctionType = apiFunction->GetFunctionType();

                wasm_externtype_t* externType = (wasm_externtype_t*)wasm_importtype_type(importType);
                wasm_functype_t*   functype = wasm_externtype_as_functype(externType);

                importFuncType.ReadFunctionTypeContext(functype);

                if (!apiFunctionType.Compare(importFuncType))
                {
                    CLogger::ErrorPrintf("Wrong function structure on import. Defined structure is '%s' [\"@%s/%s\"]\n", apiFunctionType.GenerateFunctionStructureText(importName).c_str(), m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());
                    goto Fail;
                }

                imports[i] = wasm_func_as_extern(apiFunction->GetFunctionContext());
                
                continue;

            CheckGlobalFunctions:

                continue;

            ImportFail:
                CLogger::ErrorPrintf("Couldn't import function '%s' in script[\"@%s/%s\"].\n", importName.c_str(), m_pContextStore->GetResource()->GetName().c_str(), fileName.c_str());
                goto Fail;
            }
        }
    }

    wasm_extern_vec_t moduleImports = WASM_ARRAY_VEC(imports);

    m_pInstance = wasm_instance_new(store, m_pModule, &moduleImports, &trap);

    if (!m_pInstance)
    {
        goto Fail;
    }

    delete imports;
    imports = NULL;

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

    CWebAssemblyFunction* wasmFunction = new CWebAssemblyFunction(m_pContextStore->GetStore(), functionType, function, m_pContextStore);
    wasmFunction->Build();

    if (!wasmFunction || !wasmFunction->GetFunctionContext())
    {
        return;
    }

    m_mpApiFunctions[functionName] = wasmFunction;
}

void CWebAssemblyScript::RegisterGlobalFunctions(const SString& functionName, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction function)
{
    if (functionName.empty())
    {
        return;
    }

    if (DoesGlobalFunctionExist(functionName))
    {
        DeleteGlobalFunction(functionName);
    }

    CWebAssemblyFunction* wasmFunction = new CWebAssemblyFunction(m_pContextStore->GetStore(), functionType, function, m_pContextStore);

    if (!wasmFunction || !wasmFunction->GetFunctionContext())
    {
        return;
    }

    m_mpGlobalFunctions[functionName] = wasmFunction;
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

SString CWebAssemblyScript::GetScriptFile()
{
    return m_strScriptFile;
}

bool CWebAssemblyScript::IsExternValid(const CWebAssemblyExtern& waExtern)
{
    return waExtern.context != NULL;
}
