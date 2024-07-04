/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyImports.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_WEB_ASSEMBLY_IMPORTS_HEADER
#define C_WEB_ASSEMBLY_IMPORTS_HEADER

#include <vector>

#include "wasm_export.h"
#include "wasm_c_api.h"

#define C_WASM_EXTERN_TYPE_FUNCTION WASM_EXTERN_FUNC
#define C_WASM_EXTERN_TYPE_GLOBAL   WASM_EXTERN_GLOBAL
#define C_WASM_EXTERN_TYPE_TABLE    WASM_EXTERN_TABLE
#define C_WASM_EXTERN_TYPE_MEMORY   WASM_EXTERN_MEMORY

#define C_WASM_VARIABLE_TYPE_I32     WASM_I32
#define C_WASM_VARIABLE_TYPE_I64     WASM_I64
#define C_WASM_VARIABLE_TYPE_F32     WASM_F32
#define C_WASM_VARIABLE_TYPE_F64     WASM_F64
#define C_WASM_VARIABLE_TYPE_ANYREF  WASM_ANYREF
#define C_WASM_VARIABLE_TYPE_FUNCREF WASM_FUNCREF

#define WEB_ASSEMBLY_API_MODULE_NAME "env"

#define WASM_MAIN_FUNCTION_NAME    "ModuleMain"
#define WASM_MALLOC_FUNCTION_NAME  "malloc"
#define WASM_REALLOC_FUNCTION_NAME "realloc"
#define WASM_FREE_FUNCTION_NAME    "free"

#define WASM_INTERNAL_FUNCTIONS_TABLE_EXPORT_NAME "__indirect_function_table"

#define WASM_COMPILER_CAN_USE_SHAERD_MEMORY 0

#define WEB_ASSEMBLY_NULL_PTR (0)

#define WebAssemblyApi(name, envVariableName, argsVariableName, resultsVariableName) CWebAssemblyTrap* name(void* envVariableName, const CWebAssemblyValueVector* argsVariableName, CWebAssemblyValueVector* resultsVariableName)

#define GetWebAssemblyEnvObject(env) ((CWebAssemblyApiEnviornment)env)
#define GetWebAssemblyEnvScript(env) (GetWebAssemblyEnvObject(env)->script)
#define GetWebAssemblyEnvMemory(env) (GetWebAssemblyEnvScript(env)->GetMemory())
#define GetWebAssemblyEnvFunctionName(env) (GetWebAssemblyEnvObject(env)->functionName)
#define GetWebAssemblyEnvContext(env) (GetWebAssemblyEnvScript(env)->GetStoreContext())
#define GetWebAssemblyEnvResource(env) (GetWebAssemblyEnvContext(env)->GetResource())
#define GetWebAssemblyLuaMain(env) (GetWebAssemblyEnvResource(env)->GetVirtualMachine())
#define GetWebAssemblyLuaVM(env) (GetWebAssemblyLuaMain(env)->GetVM())

#define IsAppOn64BitVersion (sizeof(intptr_t) == 8)

#if _WIN64
#define IS_APP_ON_64_BIT_VERSION 1
#else
#define IS_APP_ON_64_BIT_VERSION 0
#endif

typedef wasm_engine_t*                 CWebAssemblyEngineContext;
typedef wasm_store_t*                  CWebAssemblyStoreContext;
typedef wasm_func_callback_with_env_t  CWebAssemblyCFunction;
typedef wasm_trap_t                    CWebAssemblyTrap;
typedef wasm_module_t*                 CWebAssemblyModuleContext;
typedef wasm_shared_module_t*          CWebAssemblySharedModuleContext;
typedef wasm_instance_t*               CWebAssemblyInstanceContext;
typedef wasm_extern_t*                 CWebAssemblyExternContext;
typedef wasm_valkind_t                 CWebAssemblyVariableKind;
typedef wasm_ref_t*                    CWebAssemblyReference;
typedef wasm_valtype_t*                CWebAssemblyVariableType;
typedef wasm_valtype_vec_t*            CWebAssemblyVariableTypeList;
typedef wasm_val_t                     CWebAssemblyValue;
typedef wasm_val_vec_t                 CWebAssemblyValueVector;
typedef std::vector<CWebAssemblyValue> CWebAssemblyValueList;
typedef wasm_func_t*                   CWebAssemblyFunctionContext;
typedef wasm_functype_t*               CWebAssemblyFunctionTypeContext;
typedef wasm_memory_t*                 CWebAssemblyMemoryContext;
typedef uint32_t                       CWebAssemblyMemoryPointerAddress;

typedef std::vector<CWebAssemblyExternContext> CWebAssemblyImports;

class CWebAssemblyScript;
typedef std::vector<CWebAssemblyScript*> CWebAssemblyScriptList;

class CWebAssemblyThread;
typedef std::vector<CWebAssemblyThread*> CWebAssemblyThreadList;

class SString;

struct CWebAssemblyApiEnvironmentObject
{
    CWebAssemblyScript* script;
    SString             functionName;
};

typedef CWebAssemblyApiEnvironmentObject* CWebAssemblyApiEnviornment;

/*struct CWebAssemblyUserData
{
    intptr_t ptr;
};*/

typedef intptr_t CWebAssemblyUserData;

#endif
