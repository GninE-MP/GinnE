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

#ifndef C_WEB_ASSEMBLY_ARG_READER
#define C_WEB_ASSEMBLY_ARG_READER

#include "WebAssemblyImports.h"

class CWebAssemblyScript;

class CWebAssemblyArgReader
{
public:
    CWebAssemblyArgReader();
    CWebAssemblyArgReader(void* env, const CWebAssemblyValueVector* args, CWebAssemblyValueVector* results);
    ~CWebAssemblyArgReader() = default;

    bool NextIsInt32();
    bool NextIsUInt32();
    bool NextIsInt64();
    bool NextIsUInt64();
    bool NextIsFloat32();
    bool NextIsFloat64();
    bool NextIsString();
    bool NextIsPointer();
    bool NextIsBoolean();

    void ReadInt32(int32_t& out, int32_t defaultValue = 0);
    void ReadUInt32(uint32_t& out, uint32_t defaultValue = 0);
    void ReadInt64(int64_t& out, int64_t defaultValue = 0);
    void ReadUInt64(uint64_t& out, uint64_t defaultValue = 0);
    void ReadFloat32(float32_t& out, float32_t defaultValue = 0.0f);
    void ReadFloat64(float64_t& out, float64_t defaultValue = 0);
    void ReadString(SString& out, SString defaultValue = "", intptr_t size = -1);
    void ReadBoolean(bool& out, bool defaultValue = false);

    template<typename PTR>
    void ReadPointer(PTR*& out, PTR* defaultValue = NULL)
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

        out = (PTR*)(m_pScript->GetMemory()->GetMemoryPhysicalPointer(ptr));
    }

    template<typename PTR>
    void WritePointer(void* ptr, PTR* value, intptr_t size = sizeof(PTR))
    {
        if (!ptr)
        {
            return;
        }

        if (!value)
        {
            return;
        }

        memcpy(ptr, (void*)value, size);
    }

    template <typename PTR>
    void WritePointer(CWebAssemblyMemoryPointerAddress ptr, PTR* value, intptr_t size = sizeof(PTR))
    {
        if (ptr == WEB_ASSEMBLY_NULL_PTR)
        {
            return;
        }

        if (!value)
        {
            return;
        }

        memcpy(m_pScript->GetMemory()->GetMemoryPhysicalPointer(ptr), (void*)value, size);
    }

    void Return(const int32_t& value);
    void Return(const uint32_t& value);
    void Return(const int64_t& value);
    void Return(const uint64_t& value);
    void Return(const float32_t& value);
    void Return(const float64_t& value);
    void Return(const SString& value);
    void Return(const bool& value);

    template<typename PTR>
    void Return(PTR* ptr, intptr_t size = -1)
    {
        if (!ptr || size == 0)
        {
            m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
            m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

            return;
        }

        if (size == -1)
        {
            size = sizeof(PTR);
        }

        void* realPtr = NULL;

        CWebAssemblyMemoryPointerAddress ptrAddress = m_pScript->GetMemory()->Malloc(size, &realPtr);

        if (!realPtr)
        {
            m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
            m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

            return;
        }

        memcpy(realPtr, (void*)ptr, size);

        m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
        m_pResults->data[0].of.i32 = ptrAddress;
    }

    bool Skip();
    void Reset();

    bool CanContinue();

    void                SetScript(CWebAssemblyScript* script);
    CWebAssemblyScript* GetScript();

    void    SetError(const SString& error);
    bool    HasError();
    SString GetError();

    void SetIndex(const int& index);
    int  GetIndex();

    CWebAssemblyValueVector* GetArguments();
    CWebAssemblyValueVector* GetResults();
    size_t                   GetArgumentCount();

    bool IsValid();

    operator bool();

private:
    CWebAssemblyScript* m_pScript;

    SString m_strError;

    int m_iIndex;

    CWebAssemblyValueVector* m_pArgs;
    CWebAssemblyValueVector* m_pResults;
};

#endif
