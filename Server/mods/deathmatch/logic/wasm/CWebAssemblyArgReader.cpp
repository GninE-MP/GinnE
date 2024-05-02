/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyArgReader.cpp
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CWebAssemblyContext.h"
#include "CWebAssemblyVariable.h"

#include "CWebAssemblyArgReader.h"

CWebAssemblyArgReader::CWebAssemblyArgReader()
{
    m_pScript = NULL;
    m_strFunctionName = "";

    m_iIndex = -1;

    m_pArgs = NULL;
    m_pResults = NULL;
}

CWebAssemblyArgReader::CWebAssemblyArgReader(void* env, const CWebAssemblyValueVector* args, CWebAssemblyValueVector* results)
{
    m_pScript = env ? GetWebAssemblyEnvScript(env) : NULL;
    m_strFunctionName = env ? GetWebAssemblyEnvFunctionName(env) : NULL;

    m_iIndex = -1;

    m_pArgs = (CWebAssemblyValueVector*)args;
    m_pResults = results;
}

bool CWebAssemblyArgReader::NextIsInt32()
{
    if (CanContinue())
    {
        return m_pArgs->data[m_iIndex + 1].kind == C_WASM_VARIABLE_TYPE_I32;
    }

    return false;
}

bool CWebAssemblyArgReader::NextIsUInt32()
{
    return NextIsInt32();
}

bool CWebAssemblyArgReader::NextIsInt64()
{
    if (CanContinue())
    {
        return m_pArgs->data[m_iIndex + 1].kind == C_WASM_VARIABLE_TYPE_I64;
    }

    return false;
}

bool CWebAssemblyArgReader::NextIsUInt64()
{
    return NextIsInt64();
}

bool CWebAssemblyArgReader::NextIsFloat32()
{
    if (CanContinue())
    {
        return m_pArgs->data[m_iIndex + 1].kind == C_WASM_VARIABLE_TYPE_F32;
    }

    return false;
}

bool CWebAssemblyArgReader::NextIsFloat64()
{
    if (CanContinue())
    {
        return m_pArgs->data[m_iIndex + 1].kind == C_WASM_VARIABLE_TYPE_F64;
    }

    return false;
}

bool CWebAssemblyArgReader::NextIsString()
{
    return NextIsInt32();
}

bool CWebAssemblyArgReader::NextIsPointer()
{
    return NextIsInt32();
}

bool CWebAssemblyArgReader::NextIsBoolean()
{
    return true;
}

bool CWebAssemblyArgReader::NextIsPointerAddress()
{
    return NextIsUInt32();
}

bool CWebAssemblyArgReader::NextIsUserData()
{
    #if IS_APP_ON_64_BIT_VERSION
        return NextIsUInt64();
    #else
        return NextIsUInt32();
    #endif
}

void CWebAssemblyArgReader::ReadInt32(int32_t& out, int32_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_I32)
    {
        out = defaultValue;
        return;
    }

    out = arg.of.i32;
}

void CWebAssemblyArgReader::ReadUInt32(uint32_t& out, uint32_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_I32)
    {
        out = defaultValue;

        return;
    }

    out = (uint32_t)arg.of.i32;
}

void CWebAssemblyArgReader::ReadInt64(int64_t& out, int64_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_I64)
    {
        out = defaultValue;
        return;
    }

    out = arg.of.i64;
}

void CWebAssemblyArgReader::ReadUInt64(uint64_t& out, uint64_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_I64)
    {
        out = defaultValue;
        return;
    }

    out = arg.of.i64;
}

void CWebAssemblyArgReader::ReadFloat32(float32_t& out, float32_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_F32)
    {
        out = defaultValue;
        return;
    }

    out = arg.of.f32;
}

void CWebAssemblyArgReader::ReadFloat64(float64_t& out, float64_t defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_F64)
    {
        out = defaultValue;
        return;
    }

    out = arg.of.f64;
}

void CWebAssemblyArgReader::ReadString(SString& out, SString defaultValue, intptr_t size)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    if (arg.kind != C_WASM_VARIABLE_TYPE_I32)
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyMemoryPointerAddress ptr = arg.of.i32;

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        out = defaultValue;
        return;
    }

    out = m_pScript->GetMemory()->UTF8ToString(ptr, size);
}

void CWebAssemblyArgReader::ReadBoolean(bool& out, bool defaultValue)
{
    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyValue arg = m_pArgs->data[m_iIndex];

    switch (arg.kind)
    {
        case C_WASM_VARIABLE_TYPE_I32:
            out = (bool)arg.of.i32;
            break;
        case C_WASM_VARIABLE_TYPE_I64:
            out = (bool)arg.of.i64;
            break;
        case C_WASM_VARIABLE_TYPE_F32:
            out = (bool)arg.of.f32;
            break;
        case C_WASM_VARIABLE_TYPE_F64:
            out = (bool)arg.of.f64;
            break;
        default:
            break;
    }
}

void CWebAssemblyArgReader::ReadPointerAddress(CWebAssemblyMemoryPointerAddress& out, CWebAssemblyMemoryPointerAddress defaultValue)
{
    ReadUInt32(out, defaultValue);
}

void CWebAssemblyArgReader::ReadFunction(CWebAssemblyFunction*& out, CWebAssemblyFunction* defaultValue)
{
    if (!out)
    {
        out = defaultValue;
        return;
    }

    if (!Skip())
    {
        out = defaultValue;
        return;
    }

    CWebAssemblyMemoryPointerAddress functionPtr = m_pArgs->data[m_iIndex].of.i32;

    if (functionPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        out = defaultValue;
        return;
    }

    out = m_pScript->GetInternalFunction(functionPtr);
}

void CWebAssemblyArgReader::ReadVector2D(CVector2D& out, CVector2D defaultValue)
{
    CWebAssemblyMemoryPointerAddress ptr;

    ReadPointerAddress(ptr);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        out = defaultValue;
        return;
    }

    struct V
    {
        float32_t x;
        float32_t y;
    };

    V* value = (V*)(m_pScript->GetMemory()->GetMemoryPhysicalPointer(ptr));

    out.fX = value->x;
    out.fY = value->y;
}

void CWebAssemblyArgReader::ReadVector3D(CVector& out, CVector defaultValue)
{
    CWebAssemblyMemoryPointerAddress ptr;

    ReadPointerAddress(ptr);

    if (ptr == WEB_ASSEMBLY_NULL_PTR)
    {
        out = defaultValue;
        return;
    }

    struct V
    {
        float32_t x;
        float32_t y;
        float32_t z;
    };

    V* value = (V*)(m_pScript->GetMemory()->GetMemoryPhysicalPointer(ptr));

    out.fX = value->x;
    out.fY = value->y;
    out.fZ = value->z;
}

CWebAssemblyTrap* CWebAssemblyArgReader::ReturnNull(SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
    m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const int32_t& value, SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
    m_pResults->data[0].of.i32 = value;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const uint32_t& value, SString errorMessage)
{
    return Return((int32_t)value);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const int64_t& value, SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I64;
    m_pResults->data[0].of.i64 = value;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const uint64_t& value, SString errorMessage)
{
    return Return((int64_t)value);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const float32_t& value, SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_F32;
    m_pResults->data[0].of.f32 = value;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const float64_t& value, SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_F64;
    m_pResults->data[0].of.f64 = value;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const SString& value, SString errorMessage)
{
    if (value.empty())
    {
        m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
        m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

        return CreateTrap(errorMessage);
    }

    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
    m_pResults->data[0].of.i32 = m_pScript->GetMemory()->StringToUTF8(value);

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(const bool& value, SString errorMessage)
{
    m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
    m_pResults->data[0].of.i32 = (int32_t)value;

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CWebAssemblyVariable value, SString errorMessage)
{
    m_pResults->data[0] = value.GetValue();

    return CreateTrap(errorMessage);
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CElement* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetID().Value()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CPlayer* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetID().Value()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CResource* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CXMLNode* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CLuaTimer* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CAccount* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CAccessControlList* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CAccessControlListGroup* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CBan* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CTextDisplay* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CTextItem* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetScriptID()));
}

CWebAssemblyTrap* CWebAssemblyArgReader::Return(CDbJobData* value, SString errorMessage)
{
    if (!value)
    {
        return ReturnNull(errorMessage);
    }

    return Return((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(value->GetId()));
}

bool CWebAssemblyArgReader::Skip()
{
    if (!CanContinue())
    {
        return false;
    }

    m_iIndex++;

    return true;
}

void CWebAssemblyArgReader::Reset()
{
    m_iIndex = -1;
}

bool CWebAssemblyArgReader::CanContinue()
{
    return m_iIndex < (int)GetArgumentCount() - 1;
}

CWebAssemblyTrap* CWebAssemblyArgReader::CreateTrap(const SString& errorMessage)
{
    if (errorMessage.empty())
    {
        return NULL;
    }

    SString errorM = "";

    if (!m_strFunctionName.empty())
    {
        errorM = "->`";
        errorM += m_strFunctionName;
        errorM += "`: ";
    }
    else
    {
        errorM = ": ";
    }

    errorM += errorMessage;
    
    wasm_name_t message;
    wasm_name_new_from_string_nt(&message, errorM.c_str());

    CWebAssemblyTrap* trap = wasm_trap_new(m_pScript->GetStoreContext()->GetStore()->GetContext(), &message);

    wasm_name_delete(&message);

    return trap;
}

void CWebAssemblyArgReader::SetScript(CWebAssemblyScript* script)
{
    m_pScript = script;
}

CWebAssemblyScript* CWebAssemblyArgReader::GetScript()
{
    return m_pScript;
}

void CWebAssemblyArgReader::SetFunctionName(const SString& functionName)
{
    m_strFunctionName = functionName;
}

SString CWebAssemblyArgReader::GetFunctionName()
{
    return m_strFunctionName;
}

void CWebAssemblyArgReader::SetError(const SString& error)
{
    m_strError = error;
}

bool CWebAssemblyArgReader::HasError()
{
    return !m_strError.empty();
}

SString CWebAssemblyArgReader::GetError()
{
    return m_strError;
}

void CWebAssemblyArgReader::SetIndex(const int& index)
{
    m_iIndex = index;
}

int CWebAssemblyArgReader::GetIndex()
{
    return m_iIndex;
}

CWebAssemblyValueVector* CWebAssemblyArgReader::GetArguments()
{
    return m_pArgs;
}

CWebAssemblyValueVector* CWebAssemblyArgReader::GetResults()
{
    return m_pResults;
}

size_t CWebAssemblyArgReader::GetArgumentCount()
{
    if (m_pArgs)
    {
        return m_pArgs->num_elems;
    }

    return 0;
}

bool CWebAssemblyArgReader::IsValid()
{
    return (m_pScript && m_pArgs && m_pResults) ? true : false;
}

CWebAssemblyArgReader::operator bool()
{
    return IsValid();
}
