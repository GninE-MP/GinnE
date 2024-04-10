/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyVariable.cpp
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include <stdarg.h>

#include "StdInc.h"

#include "CWebAssemblyContext.h"
#include "CWebAssemblyVariable.h"

CWebAssemblyVariable::CWebAssemblyVariable()
{
    Set((int32_t)0);
}

CWebAssemblyVariable::CWebAssemblyVariable(int32_t value)
{
    Set(value);
}

CWebAssemblyVariable::CWebAssemblyVariable(int64_t value)
{
    Set(value);
}

CWebAssemblyVariable::CWebAssemblyVariable(float32_t value)
{
    Set(value);
}

CWebAssemblyVariable::CWebAssemblyVariable(float64_t value)
{
    Set(value);
}

CWebAssemblyVariable::CWebAssemblyVariable(CWebAssemblyReference value)
{
    Set(value);
}

CWebAssemblyVariable::CWebAssemblyVariable(CWebAssemblyValue value)
{
    Set(value);
}

void CWebAssemblyVariable::CopyVariableTypeInfo(CWebAssemblyVariableType* type)
{
    if (!type)
    {
        return;
    }

    *type = wasm_valtype_new(m_sVariable.kind);
}

CWebAssemblyVariableType CWebAssemblyVariable::GetVariableTypeInfo()
{
    return wasm_valtype_new(m_sVariable.kind);
}

void CWebAssemblyVariable::Set(int32_t value)
{
    m_sVariable.kind = C_WASM_VARIABLE_TYPE_I32;
    m_sVariable.of.i32 = value;
}

void CWebAssemblyVariable::Set(int64_t value)
{
    m_sVariable.kind = C_WASM_VARIABLE_TYPE_I64;
    m_sVariable.of.i64 = value;
}

void CWebAssemblyVariable::Set(float32_t value)
{
    m_sVariable.kind = C_WASM_VARIABLE_TYPE_F32;
    m_sVariable.of.f32 = value;
}

void CWebAssemblyVariable::Set(float64_t value)
{
    m_sVariable.kind = C_WASM_VARIABLE_TYPE_F64;
    m_sVariable.of.f64 = value;
}

void CWebAssemblyVariable::Set(CWebAssemblyReference value)
{
    m_sVariable.kind = C_WASM_VARIABLE_TYPE_ANYREF;
    m_sVariable.of.ref = value;
}

void CWebAssemblyVariable::Set(CWebAssemblyValue value)
{
    m_sVariable.kind = value.kind;
    m_sVariable.of = value.of;
}

int32_t CWebAssemblyVariable::GetInt32()
{
    return m_sVariable.of.i32;
}

int64_t CWebAssemblyVariable::GetInt64()
{
    return m_sVariable.of.i64;
}

float32_t CWebAssemblyVariable::GetFloat32()
{
    return m_sVariable.of.f32;
}

float64_t CWebAssemblyVariable::GetFloat64()
{
    return m_sVariable.of.f64;
}

CWebAssemblyReference CWebAssemblyVariable::GetReference()
{
    return m_sVariable.of.ref;
}

CWebAssemblyVariableKind CWebAssemblyVariable::GetType()
{
    return m_sVariable.kind;
}

CWebAssemblyValue& CWebAssemblyVariable::GetValue()
{
    return m_sVariable;
}

bool CWebAssemblyVariable::IsNumber()
{
    wasm_valkind_t kind = m_sVariable.kind;

    return (
        kind == C_WASM_VARIABLE_TYPE_I32 ||
        kind == C_WASM_VARIABLE_TYPE_I64 ||
        kind == C_WASM_VARIABLE_TYPE_F32 ||
        kind == C_WASM_VARIABLE_TYPE_F64
    );
}

bool CWebAssemblyVariable::IsReference()
{
    wasm_valkind_t kind = m_sVariable.kind;

    return (
        kind == C_WASM_VARIABLE_TYPE_FUNCREF ||
        kind == C_WASM_VARIABLE_TYPE_ANYREF
    );
}

bool CWebAssemblyVariable::IsInt()
{
    wasm_valkind_t kind = m_sVariable.kind;

    return (
        kind == C_WASM_VARIABLE_TYPE_I32 ||
        kind == C_WASM_VARIABLE_TYPE_I64
    );
}

bool CWebAssemblyVariable::IsInt32()
{
    return m_sVariable.kind == C_WASM_VARIABLE_TYPE_I32;
}

bool CWebAssemblyVariable::IsInt64()
{
    return m_sVariable.kind == C_WASM_VARIABLE_TYPE_I64;
}

bool CWebAssemblyVariable::IsFloat()
{
    wasm_valkind_t kind = m_sVariable.kind;

    return (
        kind == C_WASM_VARIABLE_TYPE_F32 ||
        kind == C_WASM_VARIABLE_TYPE_F64
    );
}

bool CWebAssemblyVariable::IsFloat32()
{
    return m_sVariable.kind == C_WASM_VARIABLE_TYPE_F32;
}

bool CWebAssemblyVariable::IsFloat64()
{
    return m_sVariable.kind == C_WASM_VARIABLE_TYPE_F64;
}

void CWebAssemblyVariable::FreeWebAssemblyVariableType(const CWebAssemblyVariableType& type)
{
    if (!type)
    {
        return;
    }

    wasm_valtype_delete(type);
}

SString CWebAssemblyVariable::GetWebAssemblyVariableTypeName(const CWebAssemblyVariableKind& kind)
{
    SString type = "Unknown";

    switch (kind)
    {
        case C_WASM_VARIABLE_TYPE_I32:
            type = "int32";
            break;
        case C_WASM_VARIABLE_TYPE_I64:
            type = "int64";
            break;
        case C_WASM_VARIABLE_TYPE_F32:
            type = "float32";
            break;
        case C_WASM_VARIABLE_TYPE_F64:
            type = "float64";
            break;
        case C_WASM_VARIABLE_TYPE_ANYREF:
            type = "reference";
            break;
        case C_WASM_VARIABLE_TYPE_FUNCREF:
            type = "function";
            break;
        default:
            break;
    }

    return type;
}

CWebAssemblyVariables::~CWebAssemblyVariables()
{
    Clear();
}

void CWebAssemblyVariables::CopyVariablesTypeInfo(CWebAssemblyVariableTypeList* list)
{
    if (!list)
    {
        return;
    }

    *list = GetVariablesTypeInfo();
}

CWebAssemblyVariableTypeList CWebAssemblyVariables::GetVariablesTypeInfo()
{
    size_t size = GetSize();

    if (size < 1)
    {
        return NULL;
    }

    CWebAssemblyVariableTypeList list = NULL;

    std::vector<CWebAssemblyVariableType> types;

    for (CWebAssemblyVariable& var : m_lsVariables)
    {
        types.push_back(var.GetVariableTypeInfo());
    }

    wasm_valtype_vec_new(list, size, types.data());

    return list;
}

void CWebAssemblyVariables::Push(const CWebAssemblyVariable& var)
{
    m_lsVariables.push_back(var);
}

void CWebAssemblyVariables::PushInt32()
{
    Push(CWebAssemblyVariable((int32_t)0));
}

void CWebAssemblyVariables::PushInt64()
{
    Push(CWebAssemblyVariable((int64_t)0));
}

void CWebAssemblyVariables::PushFloat32()
{
    Push(CWebAssemblyVariable((float32_t)0));
}

void CWebAssemblyVariables::PushFloat64()
{
    Push(CWebAssemblyVariable((float64_t)0));
}

void CWebAssemblyVariables::Pop()
{
    m_lsVariables.pop_back();
}

void CWebAssemblyVariables::Clear()
{
    m_lsVariables.clear();
}

void CWebAssemblyVariables::WriteValueList(CWebAssemblyValueList* list)
{
    if (!list)
    {
        return;
    }

    size_t length = GetSize();

    if (length < 1)
    {
        return;
    }

    for (CWebAssemblyVariable& var : m_lsVariables)
    {
        list->push_back(var.GetValue());
    }
}

size_t CWebAssemblyVariables::GetSize()
{
    return m_lsVariables.size();
}

CWebAssemblyVariable& CWebAssemblyVariables::Get(int32_t index)
{
    return m_lsVariables.at(index);
}

CWebAssemblyVariable& CWebAssemblyVariables::GetFirst()
{
    return m_lsVariables.front();
}

CWebAssemblyVariable& CWebAssemblyVariables::GetLast()
{
    return m_lsVariables.back();
}

CWebAssemblyVariableList& CWebAssemblyVariables::GetList()
{
    return m_lsVariables;
}

bool CWebAssemblyVariables::IsEmpty()
{
    return m_lsVariables.empty();
}

CWebAssemblyVariable& CWebAssemblyVariables::operator[](int32_t index)
{
    return m_lsVariables[index];
}

void CWebAssemblyVariables::FreeWebAssemblyVariablesTypeList(const CWebAssemblyVariableTypeList& list)
{
    if (!list)
    {
        return;
    }

    wasm_valtype_vec_delete(list);
}

CWebAssemblyFunctionType::CWebAssemblyFunctionType(const CWebAssemblyVariables& args, const CWebAssemblyVariables& results)
{
    SetArguments(args);
    SetResults(results);
}

CWebAssemblyFunctionType::~CWebAssemblyFunctionType()
{
    m_wavArgs.Clear();
    m_wavResults.Clear();
}

void CWebAssemblyFunctionType::WriteWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext* context)
{
    if (!context)
    {
        return;
    }

    CWebAssemblyVariableTypeList args;
    CWebAssemblyVariableTypeList results;

    m_wavArgs.CopyVariablesTypeInfo(&args);
    m_wavResults.CopyVariablesTypeInfo(&results);

    if (!args || !results)
    {
        return;
    }

    *context = wasm_functype_new(args, results);
}

void CWebAssemblyFunctionType::SetArguments(const CWebAssemblyVariables& args)
{
    m_wavArgs.Clear();

    m_wavArgs = args;
}

void CWebAssemblyFunctionType::SetResults(const CWebAssemblyVariables& results)
{
    m_wavResults.Clear();

    m_wavResults = results;
}

CWebAssemblyVariables& CWebAssemblyFunctionType::GetArguments()
{
    return m_wavArgs;
}

CWebAssemblyVariables& CWebAssemblyFunctionType::GetResults()
{
    return m_wavResults;
}

void CWebAssemblyFunctionType::FreeWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext context)
{
    if (!context)
    {
        return;
    }

    wasm_functype_delete(context);
}
/*
CWebAssemblyFunction::CWebAssemblyFunction()
{
    m_pContext = NULL;
    m_pCFunction = NULL;
}

CWebAssemblyFunction::CWebAssemblyFunction(const CWebAssemblyFunctionContext& context)
{
    SetContext(context);
}

CWebAssemblyFunction::~CWebAssemblyFunction()
{
    Free();
}

void CWebAssemblyFunction::SetContext(const CWebAssemblyFunctionContext& context)
{
    m_pContext = context;
}

CWebAssemblyFunctionContext CWebAssemblyFunction::GetContext()
{
    return m_pContext;
}

void CWebAssemblyFunction::SetFunctionType(CWebAssemblyFunctionType functionType)
{
    m_waFunctionType = functionType;
}

CWebAssemblyFunctionType CWebAssemblyFunction::GetFunctionType()
{
    return m_waFunctionType;
}

void CWebAssemblyFunction::SetCFunction(const CWebAssemblyCFunction& cFunction)
{
    m_pCFunction = cFunction;
}

CWebAssemblyCFunction CWebAssemblyFunction::GetCFunction()
{
    return m_pCFunction;
}

void CWebAssemblyFunction::Free()
{
    if (m_pContext)
    {
        return;
    }

    wasm_func_delete(m_pContext);


}
*/

CWebAssemblyFunction::CWebAssemblyFunction()
{
    m_pContext = NULL;
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore store)
{
    SetStore(store);
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore store, CWebAssemblyFunctionType functionType)
{
    SetStore(store);
    SetFunctionType(functionType);
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction)
{
    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);
}

CWebAssemblyFunction::CWebAssemblyFunction(const CWebAssemblyFunctionContext& context)
{
    SetFunctionContext(context);
}

CWebAssemblyFunction::~CWebAssemblyFunction()
{
    Free();
}

void CWebAssemblyFunction::TakeOwenerShip(CWebAssemblyFunction function)
{
    CWebAssemblyFunctionContext context = function.GetFunctionContext();

    if (!context)
    {
        return;
    }

    CWebAssemblyFunctionType functionType = function.GetFunctionType();
    CWebAssemblyStore        engineStore = function.GetStore();

    function.Drop();

    m_pContext = context;
    m_waFunctionType = functionType;

    m_waStore = engineStore;
}

void CWebAssemblyFunction::Drop()
{
    m_pContext = NULL;

    m_waFunctionType = CWebAssemblyFunctionType();
    m_waStore = CWebAssemblyStore();
}

void CWebAssemblyFunction::Build(CWebAssemblyFunctionType functionType)
{
    SetFunctionType(functionType);

    Build();
}

void CWebAssemblyFunction::Build(CWebAssemblyStore store, CWebAssemblyFunctionType functionType)
{
    SetStore(store);
    SetFunctionType(functionType);

    Build();
}

void CWebAssemblyFunction::Build(CWebAssemblyStore store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction)
{
    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);

    Build();
}

void CWebAssemblyFunction::Build()
{
    if (!m_waStore)
    {
        return;
    }

    if (!m_pCFunction)
    {
        return;
    }

    CWebAssemblyFunctionTypeContext functionType;
    m_waFunctionType.WriteWebAssemblyFunctionTypeContext(&functionType);

    if (!functionType)
    {
        return;
    }

    m_pContext = wasm_func_new(m_waStore.GetContext(), functionType, m_pCFunction);
}

CWebAssemblyTrap* CWebAssemblyFunction::Call(CWebAssemblyVariables* args, CWebAssemblyVariables* results)
{
    if (!m_pContext)
    {
        return NULL;
    }

    CWebAssemblyValueList argsValues;

    if (args)
    {
        args->WriteValueList(&argsValues);
    }

    wasm_val_vec_t argsVector;
    wasm_val_vec_t resultsVector;

    resultsVector.data = NULL;

    if (args)
    {
        wasm_val_vec_new(&argsVector, argsValues.size(), argsValues.data());
    }

    CWebAssemblyTrap* trap = wasm_func_call(m_pContext, &argsVector, &resultsVector);

    if (results)
    {
        size_t length = resultsVector.num_elems;

        for (size_t i = 0; i < length; i++)
        {
            results->Push(resultsVector.data[i]);
        }

        wasm_val_vec_delete(&resultsVector);
    }

    if (resultsVector.data)
    {
        wasm_val_vec_delete(&resultsVector);
    }

    if (args)
    {
        wasm_val_vec_delete(&argsVector);
    }

    return trap;
}

void CWebAssemblyFunction::SetFunctionContext(const CWebAssemblyFunctionContext& context)
{
    if (!context)
    {
        return;
    }

    Free();

    m_pContext = context;
}

CWebAssemblyFunctionContext CWebAssemblyFunction::GetFunctionContext()
{
    return m_pContext;
}

void CWebAssemblyFunction::SetFunctionType(CWebAssemblyFunctionType type)
{
    m_waFunctionType = type;
}

CWebAssemblyFunctionType CWebAssemblyFunction::GetFunctionType()
{
    return m_waFunctionType;
}

void CWebAssemblyFunction::SetStore(CWebAssemblyStore store)
{
    if (!store)
    {
        return;
    }

    m_waStore = store;
}

CWebAssemblyStore CWebAssemblyFunction::GetStore()
{
    return m_waStore;
}

void CWebAssemblyFunction::SetCFunction(CWebAssemblyCFunction cFunction)
{
    m_pCFunction = cFunction;
}

CWebAssemblyCFunction CWebAssemblyFunction::GetCFunction()
{
    return m_pCFunction;
}

void CWebAssemblyFunction::Free()
{
    if (!m_pContext)
    {
        return;
    }

    wasm_func_delete(m_pContext);

    Drop();
}

CWebAssemblyFunction::operator bool()
{
    return m_pContext ? true : false;
}
