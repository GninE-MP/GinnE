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

void CWebAssemblyVariables::CopyVariablesTypeInfo(CWebAssemblyVariableTypeList list)
{
    if (!list)
    {
        return;
    }

    size_t size = GetSize();

    if (size < 1)
    {
        wasm_valtype_vec_new_empty(list);
        return;
    }

    std::vector<CWebAssemblyVariableType> types;

    for (CWebAssemblyVariable& var : m_lsVariables)
    {
        types.push_back(var.GetVariableTypeInfo());
    }

    wasm_valtype_vec_new(list, size, types.data());
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

void CWebAssemblyVariables::PushAnyRef()
{
    CWebAssemblyValue value;

    value.kind = C_WASM_VARIABLE_TYPE_ANYREF;
    value.of.ref = NULL;

    CWebAssemblyVariable var(value);

    m_lsVariables.push_back(var);
}

void CWebAssemblyVariables::PushFunctionRef()
{
    CWebAssemblyValue value;

    value.kind = C_WASM_VARIABLE_TYPE_FUNCREF;
    value.of.foreign = NULL;

    CWebAssemblyVariable var(value);

    m_lsVariables.push_back(var);
}

void CWebAssemblyVariables::PushPointer()
{
    PushInt32();
}

void CWebAssemblyVariables::PushString()
{
    PushInt32();
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

CWebAssemblyFunctionType::CWebAssemblyFunctionType(const CWebAssemblyFunctionTypeContext& context)
{
    ReadFunctionTypeContext(context);
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

void CWebAssemblyFunctionType::ReadFunctionTypeContext(const CWebAssemblyFunctionTypeContext& context)
{
    if (!context)
    {
        return;
    }

    m_wavArgs.Clear();
    m_wavResults.Clear();

    const wasm_valtype_vec_t* args = wasm_functype_params(context);
    const wasm_valtype_vec_t* results = wasm_functype_results(context);
    
    size_t argsLength = args->num_elems;

    for (size_t i = 0; i < argsLength; i++)
    {
        wasm_valtype_t* valType = args->data[i];

        switch (wasm_valtype_kind(valType))
        {
            case C_WASM_VARIABLE_TYPE_I32:
                m_wavArgs.PushInt32();
                break;
            case C_WASM_VARIABLE_TYPE_I64:
                m_wavArgs.PushInt64();
                break;
            case C_WASM_VARIABLE_TYPE_F32:
                m_wavArgs.PushFloat32();
                break;
            case C_WASM_VARIABLE_TYPE_F64:
                m_wavArgs.PushFloat64();
                break;
            case C_WASM_VARIABLE_TYPE_ANYREF:
                m_wavArgs.PushAnyRef();
                break;
            case C_WASM_VARIABLE_TYPE_FUNCREF:
                m_wavArgs.PushFunctionRef();
                break;
            default:
                break;
        }
    }

    size_t resultsLength = results->num_elems;

    for (size_t i = 0; i < resultsLength; i++)
    {
        wasm_valtype_t* valType = results->data[i];

        switch (wasm_valtype_kind(valType))
        {
            case C_WASM_VARIABLE_TYPE_I32:
                m_wavResults.PushInt32();
                break;
            case C_WASM_VARIABLE_TYPE_I64:
                m_wavResults.PushInt64();
                break;
            case C_WASM_VARIABLE_TYPE_F32:
                m_wavResults.PushFloat32();
                break;
            case C_WASM_VARIABLE_TYPE_F64:
                m_wavResults.PushFloat64();
                break;
            case C_WASM_VARIABLE_TYPE_ANYREF:
                m_wavResults.PushAnyRef();
                break;
            case C_WASM_VARIABLE_TYPE_FUNCREF:
                m_wavResults.PushFunctionRef();
                break;
            default:
                break;
        }
    }
}

void CWebAssemblyFunctionType::WriteWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext* context)
{
    if (!context)
    {
        return;
    }

    wasm_valtype_vec_t args;
    wasm_valtype_vec_t results;

    m_wavArgs.CopyVariablesTypeInfo(&args);
    m_wavResults.CopyVariablesTypeInfo(&results);
    
    *context = wasm_functype_new(&args, &results);
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

bool CWebAssemblyFunctionType::Compare(CWebAssemblyFunctionType functionType)
{
    CWebAssemblyVariables& args = functionType.GetArguments();
    CWebAssemblyVariables& results = functionType.GetResults();
    
    size_t argsLength = args.GetSize();
    size_t resultsLength = results.GetSize();
    size_t selfArgsLength = m_wavArgs.GetSize();
    size_t selfResultsLength = m_wavResults.GetSize();
    
    if (argsLength != selfArgsLength)
    {
        return false;
    }

    if (resultsLength != selfResultsLength)
    {
        return false;
    }
    
    for (size_t i = 0; i < argsLength; i++)
    {
        if (args[i].GetType() != m_wavArgs[i].GetType())
        {
            return false;
        }
    }
    
    for (size_t i = 0; i < resultsLength; i++)
    {
        if (results[i].GetType() != m_wavResults[i].GetType())
        {
            return false;
        }
    }
    
    return true;
}

SString CWebAssemblyFunctionType::GenerateFunctionStructureText(const SString& functionName)
{
    SString result = "";
    SString cama = ", ";
    SString arguments = "";
    SString returns = "";

    size_t camaLength = cama.length();
    size_t argsLength = m_wavArgs.GetSize();
    size_t resultsLength = m_wavResults.GetSize();

    if (argsLength < 1)
    {
        arguments = "void";
    }
    else
    {
        for (size_t i = 0; i < argsLength; i++)
        {
            SString typeName = CWebAssemblyVariable::GetWebAssemblyVariableTypeName(m_wavArgs[i].GetType());

            arguments += typeName;
            arguments += cama;
        }

        if (!arguments.empty())
        {
            arguments = arguments.substr(0, arguments.length() - camaLength);
        }
    }

    if (resultsLength < 1)
    {
        returns = "void";
    }
    else
    {
        for (size_t i = 0; i < resultsLength; i++)
        {
            SString typeName = CWebAssemblyVariable::GetWebAssemblyVariableTypeName(m_wavResults[i].GetType());

            returns += typeName;
            returns += cama;
        }

        if (!returns.empty())
        {
            returns = returns.substr(0, returns.length() - camaLength);
        }
    }

    result += returns;
    result += " ";
    result += functionName;
    result += "(";
    result += arguments;
    result += ")";

    return result;
}

void CWebAssemblyFunctionType::FreeWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext context)
{
    if (!context)
    {
        return;
    }

    wasm_functype_delete(context);
}

CWebAssemblyFunction::CWebAssemblyFunction()
{
    Drop();
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore* store)
{
    Drop();

    SetStore(store);
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType)
{
    Drop();

    SetStore(store);
    SetFunctionType(functionType);
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction)
{
    Drop();

    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);
}

CWebAssemblyFunction::CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction, CWebAssemblyApiEnviornment enviornment)
{
    Drop();

    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);
    SetApiEnviornment(enviornment);
}

CWebAssemblyFunction::CWebAssemblyFunction(const CWebAssemblyFunctionContext& context)
{
    Drop();

    SetFunctionContext(context);
}

CWebAssemblyFunction::~CWebAssemblyFunction()
{
    Free();
}

void CWebAssemblyFunction::TakeOwenerShip(CWebAssemblyFunction* function)
{
    if (!function)
    {
        return;
    }

    CWebAssemblyFunctionContext context = function->GetFunctionContext();

    if (!context)
    {
        return;
    }

    CWebAssemblyFunctionType   functionType = function->GetFunctionType();
    CWebAssemblyStore*         engineStore = function->GetStore();
    CWebAssemblyApiEnviornment environment = function->GetApiEnviornment();

    function->Drop();

    m_pContext = context;
    m_waFunctionType = functionType;

    m_pStore = engineStore;

    m_pEnvironment = environment;
}

void CWebAssemblyFunction::Drop()
{
    m_pContext = NULL;

    m_waFunctionType = CWebAssemblyFunctionType();

    m_pCFunction = NULL;
    m_pEnvironment = NULL;
    m_pCustomEnv = NULL;
    m_pStore = NULL;
}

void CWebAssemblyFunction::Build(CWebAssemblyFunctionType functionType)
{
    SetFunctionType(functionType);

    Build();
}

void CWebAssemblyFunction::Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType)
{
    SetStore(store);
    SetFunctionType(functionType);

    Build();
}

void CWebAssemblyFunction::Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction)
{
    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);

    Build();
}

void CWebAssemblyFunction::Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction, CWebAssemblyApiEnviornment enviornment)
{
    SetStore(store);
    SetFunctionType(functionType);
    SetCFunction(cFunction);
    SetApiEnviornment(enviornment);

    Build();
}

void CWebAssemblyFunction::Build(void* customEnviornment)
{
    if (!m_pStore)
    {
        return;
    }

    if (!m_pCFunction)
    {
        return;
    }

    void* env = !customEnviornment ? (void*)m_pEnvironment : customEnviornment;

    if (!env)
    {
        return;
    }

    CWebAssemblyFunctionTypeContext functionType;
    m_waFunctionType.WriteWebAssemblyFunctionTypeContext(&functionType);

    if (!functionType)
    {
        return;
    }

    if (customEnviornment)
    {
        m_pCustomEnv = customEnviornment;
    }

    m_pContext = wasm_func_new_with_env(m_pStore->GetContext(), functionType, m_pCFunction, env, NULL);
}

bool CWebAssemblyFunction::Call(CWebAssemblyVariables* args, CWebAssemblyVariables* results, SString* errorMessage)
{
    if (!m_pContext)
    {
        return false;
    }

    CWebAssemblyVariables& functionArguments = m_waFunctionType.GetArguments();
    CWebAssemblyVariables& functionResults = m_waFunctionType.GetResults();

    CWebAssemblyValueList argsValues;

    if (args)
    {
        args->WriteValueList(&argsValues);
    }
    
    int difference = (int)argsValues.size() - (int)functionArguments.GetSize();

    if (difference > 0)
    {
        for (int i = 0; i < difference; i++, argsValues.pop_back());
    }
    else if (difference < 0)
    {
        int count = functionArguments.GetSize();

        for (int i = argsValues.size(); i < count; i++)
        {            
            argsValues.push_back(functionArguments[i].GetValue());
        }
    }

    wasm_val_vec_t argsVector = { 0 };
    wasm_val_vec_t resultsVector = { 0 };

    size_t functionResultsLength = functionResults.GetSize();

    if (functionResultsLength < 1)
    {
        wasm_val_vec_new_empty(&resultsVector);
    }
    else
    {
        CWebAssemblyValueList resultsValues;

        functionResults.WriteValueList(&resultsValues);

        wasm_val_vec_new(&resultsVector, resultsValues.size(), resultsValues.data());
    }

    size_t argsValuesLength = argsValues.size();

    if (argsValuesLength > 0)
    {
        wasm_val_vec_new(&argsVector, argsValuesLength, argsValues.data());
    }

    CWebAssemblyTrap* trap = wasm_func_call(m_pContext, &argsVector, &resultsVector);

    if (resultsVector.data)
    {
        if (results)
        {
            size_t length = resultsVector.num_elems;

            for (size_t i = 0; i < length; i++)
            {
                results->Push(resultsVector.data[i]);
            }
        }

        wasm_val_vec_delete(&resultsVector);
    }

    if (args)
    {
        wasm_val_vec_delete(&argsVector);
    }

    if (trap)
    {
        wasm_name_t message;
        wasm_trap_message(trap, &message);

        wasm_frame_t* frame = wasm_trap_origin(trap);

        char frameData[0x5DC];
        int  frameDataLength = 0;

        if (frame)
        {
            frameDataLength = sprintf(frameData, "%p @ 0x%zx = %u.0x%zx: ", wasm_frame_instance(frame), wasm_frame_module_offset(frame), wasm_frame_func_index(frame), wasm_frame_func_offset(frame));

            wasm_frame_delete(frame);
        }
        else
        {
            if (m_strFunctionName.empty())
            {
                frameDataLength = sprintf(frameData, " ");
            }
            else
            {
                frameDataLength = sprintf(frameData, "`%s`", m_strFunctionName.c_str());
            }
        }

        *errorMessage = std::string(frameData, frameDataLength);

        std::string messageData(message.data, message.num_elems);

        *errorMessage += messageData.substr(sizeof("Exception:") - 1);

        wasm_trap_delete(trap);
        wasm_name_delete(&message);

        return false;
    }

    return true;
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

void CWebAssemblyFunction::SetApiEnviornment(CWebAssemblyApiEnviornment enviornment)
{
    m_pEnvironment = enviornment;
}

CWebAssemblyApiEnviornment CWebAssemblyFunction::GetApiEnviornment()
{
    return m_pEnvironment;
}

void CWebAssemblyFunction::SetStore(CWebAssemblyStore* store)
{
    m_pStore = store;
}

CWebAssemblyStore* CWebAssemblyFunction::GetStore()
{
    return m_pStore;
}

void CWebAssemblyFunction::SetFunctionName(const SString& functionName)
{
    m_strFunctionName = functionName;
}

SString CWebAssemblyFunction::GetFunctionName()
{
    return m_strFunctionName;
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

    if (m_pEnvironment)
    {
        delete m_pEnvironment;
    }

    if (m_pCustomEnv)
    {
        delete m_pCustomEnv;
    } 
    
    Drop();
}

CWebAssemblyFunction::operator bool()
{
    return m_pContext ? true : false;
}
