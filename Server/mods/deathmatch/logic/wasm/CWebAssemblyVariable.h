/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyVariable.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_WEB_ASSEMBLY_VARIABLE_HEADER
#define C_WEB_ASSEMBLY_VARIABLE_HEADER

#include <vector>

#include "WebAssemblyImports.h"

class CWebAssemblyVariable
{
public:
    CWebAssemblyVariable();
    CWebAssemblyVariable(int32_t value);
    CWebAssemblyVariable(int64_t value);
    CWebAssemblyVariable(float32_t value);
    CWebAssemblyVariable(float64_t value);
    CWebAssemblyVariable(CWebAssemblyReference value);
    CWebAssemblyVariable(CWebAssemblyValue value);
    ~CWebAssemblyVariable() = default;
    
    void                     CopyVariableTypeInfo(CWebAssemblyVariableType* type);
    CWebAssemblyVariableType GetVariableTypeInfo();

    void Set(int32_t value);
    void Set(int64_t value);
    void Set(float32_t value);
    void Set(float64_t value);
    void Set(CWebAssemblyReference value);
    void Set(CWebAssemblyValue value);

    int32_t               GetInt32();
    int64_t               GetInt64();
    float32_t             GetFloat32();
    float64_t             GetFloat64();
    CWebAssemblyReference GetReference();

    CWebAssemblyVariableKind GetType();

    CWebAssemblyValue& GetValue();

    bool IsNumber();
    bool IsReference();

    bool IsInt();
    bool IsInt32();
    bool IsInt64();

    bool IsFloat();
    bool IsFloat32();
    bool IsFloat64();

    static void    FreeWebAssemblyVariableType(const CWebAssemblyVariableType& type);
    static SString GetWebAssemblyVariableTypeName(const CWebAssemblyVariableKind& kind);

private:
    CWebAssemblyValue m_sVariable;
};

typedef std::vector<CWebAssemblyVariable> CWebAssemblyVariableList;

class CWebAssemblyVariables
{
public:
    CWebAssemblyVariables() = default;
    ~CWebAssemblyVariables();

    void Push(const CWebAssemblyVariable& var);
    void PushInt32();
    void PushInt64();
    void PushFloat32();
    void PushFloat64();
    void PushAnyRef();
    void PushFunctionRef();
    void PushPointer();
    void PushString();

    void Pop();

    void Clear();

    void                         CopyVariablesTypeInfo(CWebAssemblyVariableTypeList list);
    CWebAssemblyVariableTypeList GetVariablesTypeInfo();

    void WriteValueList(CWebAssemblyValueList* list);

    size_t GetSize();

    CWebAssemblyVariable& Get(int32_t index);
    CWebAssemblyVariable& GetFirst();
    CWebAssemblyVariable& GetLast();

    CWebAssemblyVariableList& GetList();

    bool IsEmpty();

    CWebAssemblyVariable& operator[](int32_t index);

    static void FreeWebAssemblyVariablesTypeList(const CWebAssemblyVariableTypeList& list);

private:
    CWebAssemblyVariableList m_lsVariables;
};

class CWebAssemblyFunction;

class CWebAssemblyFunctionType
{
public:
    CWebAssemblyFunctionType() = default;
    CWebAssemblyFunctionType(const CWebAssemblyFunctionTypeContext& context);
    CWebAssemblyFunctionType(const CWebAssemblyVariables& args, const CWebAssemblyVariables& results);
    ~CWebAssemblyFunctionType();

    void ReadFunctionTypeContext(const CWebAssemblyFunctionTypeContext& context);

    void WriteWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext* context);

    void SetArguments(const CWebAssemblyVariables& args);
    void SetResults(const CWebAssemblyVariables& results);

    CWebAssemblyVariables& GetArguments();
    CWebAssemblyVariables& GetResults();

    bool Compare(CWebAssemblyFunctionType functionType);

    SString GenerateFunctionStructureText(const SString& functionName);

    static void FreeWebAssemblyFunctionTypeContext(CWebAssemblyFunctionTypeContext context);

private:
    CWebAssemblyVariables m_wavArgs;
    CWebAssemblyVariables m_wavResults;
};

class CWebAssemblyStore;

class CWebAssemblyFunction
{
public:
    CWebAssemblyFunction();
    CWebAssemblyFunction(CWebAssemblyStore* store);
    CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType);
    CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction);
    CWebAssemblyFunction(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction, CWebAssemblyApiEnviornment environment);
    CWebAssemblyFunction(const CWebAssemblyFunctionContext& context);
    ~CWebAssemblyFunction();

    void TakeOwenerShip(CWebAssemblyFunction* function);
    void Drop();

    void Build(CWebAssemblyFunctionType functionType);
    void Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType);
    void Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction);
    void Build(CWebAssemblyStore* store, CWebAssemblyFunctionType functionType, CWebAssemblyCFunction cFunction, CWebAssemblyApiEnviornment environment);
    void Build();

    bool Call(CWebAssemblyVariables* args, CWebAssemblyVariables* results);

    void                        SetFunctionContext(const CWebAssemblyFunctionContext& context);
    CWebAssemblyFunctionContext GetFunctionContext();

    void                     SetFunctionType(CWebAssemblyFunctionType functionType);
    CWebAssemblyFunctionType GetFunctionType();

    void                  SetCFunction(CWebAssemblyCFunction cFunction);
    CWebAssemblyCFunction GetCFunction();

    void                       SetApiEnviornment(CWebAssemblyApiEnviornment environment);
    CWebAssemblyApiEnviornment GetApiEnviornment();

    void               SetStore(CWebAssemblyStore* store);
    CWebAssemblyStore* GetStore();

    void Free();

    operator bool();

private:
    CWebAssemblyFunctionContext m_pContext;
    CWebAssemblyFunctionType    m_waFunctionType;
    CWebAssemblyCFunction       m_pCFunction;

    CWebAssemblyApiEnviornment m_pEnvironment;

    CWebAssemblyStore* m_pStore;
};

#endif
