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

#define WebAssemblyApi(name, envVariableName, argsVariableName, resultsVariableName) CWebAssemblyTrap* name(void* envVariableName, const CWebAssemblyValueVector* argsVariableName, CWebAssemblyValueVector* resultsVariableName)

#define GetWebAssemblyApiEnviornment(env) ((CWebAssemblyContext*)env)
#define GetWebAssemblyResource(env)       (((CWebAssemblyContext*)env)->GetResource())

typedef wasm_engine_t*                 CWebAssemblyEngineContext;
typedef wasm_store_t*                  CWebAssemblyStoreContext;
typedef wasm_func_callback_with_env_t  CWebAssemblyCFunction;
typedef wasm_trap_t                    CWebAssemblyTrap;
typedef wasm_module_t*                 CWebAssemblyModuleContext;
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

typedef std::vector<CWebAssemblyExternContext> CWebAssemblyImports;

class CWebAssemblyScript;
typedef std::vector<CWebAssemblyScript*> CWebAssemblyScriptList;

class CWebAssemblyContext;
typedef CWebAssemblyContext* CWebAssemblyApiEnviornment;

#endif
