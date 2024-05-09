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
#include "lua/CLuaFunctionParseHelpers.h"
#include "lua/CLuaMain.h"
#include "CBan.h"
#include "CAccessControlList.h"
#include "CAccessControlListGroup.h"
#include "CDatabaseManager.h"

#define ELEMENT_TO_WASM_USERDATA(element) ((CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>((element)->GetID().Value()))

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
    bool NextIsPointerAddress();
    bool NextIsUserData();

    void ReadInt32(int32_t& out, int32_t defaultValue = 0);
    void ReadUInt32(uint32_t& out, uint32_t defaultValue = 0);
    void ReadInt64(int64_t& out, int64_t defaultValue = 0);
    void ReadUInt64(uint64_t& out, uint64_t defaultValue = 0);
    void ReadFloat32(float32_t& out, float32_t defaultValue = 0.0f);
    void ReadFloat64(float64_t& out, float64_t defaultValue = 0);
    void ReadString(SString& out, SString defaultValue = "", intptr_t size = -1);
    void ReadBoolean(bool& out, bool defaultValue = false);
    void ReadPointerAddress(CWebAssemblyMemoryPointerAddress& out, CWebAssemblyMemoryPointerAddress defaultValue = WEB_ASSEMBLY_NULL_PTR);
    void ReadFunction(CWebAssemblyFunction*& out, CWebAssemblyFunction* defaultValue = NULL);
    void ReadVector2D(CVector2D& out, CVector2D defaultValue = CVector2D(0.0f, 0.0f));
    void ReadVector3D(CVector& out, CVector defaultValue = CVector(0.0f, 0.0f, 0.0f));

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
    void ReadSystemPointer(PTR*& out, PTR* defaultValue = NULL)
    {
        CWebAssemblyUserData userdata;

        #if IS_APP_ON_64_BIT_VERSION
            ReadInt64(userdata);
        #else
            ReadInt32(userdata);
        #endif

        if (!userdata)
        {
            out = defaultValue;
            return;
        }

        PTR* udata = (PTR*)(void*)userdata;

        if (!udata)
        {
            out = defaultValue;
            return;
        }

        out = udata;
    }

    template<typename PTR>
    void ReadUserData(PTR*& out, PTR* defaultValue = NULL)
    {
        CWebAssemblyUserData userdata;

        #if IS_APP_ON_64_BIT_VERSION
            ReadInt64(userdata);
        #else
            ReadInt32(userdata);
        #endif

        if (!userdata)
        {
            out = defaultValue;
            return;
        }

        lua_State* luaVM = m_pScript->GetStoreContext()->GetResource()->GetVirtualMachine()->GetVM();
        PTR* udata = (PTR*)UserDataCast((PTR*)(void*)userdata, luaVM);

        if (!udata)
        {
            out = defaultValue;
            return;
        }

        out = udata; 
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

    bool HasResult();

    CWebAssemblyTrap* ReturnNull(SString errorMessage = "");
    CWebAssemblyTrap* Return(const int32_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const uint32_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const int64_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const uint64_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const float32_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const float64_t& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const SString& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(const bool& value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CWebAssemblyVariable value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CElement* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CPlayer* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CResource* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CXMLNode* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CLuaTimer* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CAccount* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CAccessControlList* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CAccessControlListGroup* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CBan* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CTextDisplay* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CTextItem* value, SString errorMessage = "");
    CWebAssemblyTrap* Return(CDbJobData* value, SString errorMessage = "");

    template<typename PTR>
    CWebAssemblyTrap* Return(PTR* ptr, intptr_t size = -1, SString errorMessage = "")
    {
        if (!HasResult())
        {
            return CreateTrap(errorMessage);
        }

        if (!ptr || size == 0)
        {
            m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
            m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

            return CreateTrap(errorMessage);
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

            return CreateTrap(errorMessage);
        }

        memcpy(realPtr, (void*)ptr, size);

        m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
        m_pResults->data[0].of.i32 = ptrAddress;

        return CreateTrap(errorMessage);
    }

    template<typename PTR>
    CWebAssemblyTrap* ReturnSystemPointer(PTR* userData, SString errorMessage = "")
    {
        if (!HasResult())
        {
            return CreateTrap(errorMessage);
        }

        return Return((CWebAssemblyUserData)(void*)(CWebAssemblyUserData*)userData, errorMessage);
    }

    CWebAssemblyTrap* ReturnUserData(void* userdata, SString errorMessage = "")
    {
        if (!HasResult())
        {
            return CreateTrap(errorMessage);
        }

        if (!userdata)
        {
            return CreateTrap(errorMessage);
        }

        m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
        m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

        if (CElement* pEntity = UserDataCast((CElement*)userdata, NULL))
        {
            if (!pEntity)
            {
                return CreateTrap(errorMessage);
            }

            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pEntity->GetID().Value());

            return CreateTrap(errorMessage);
        }

        lua_State* luaVM = m_pScript->GetStoreContext()->GetResource()->GetVirtualMachine()->GetVM();

        if (CPlayer* pEntity = UserDataCast((CPlayer*)userdata, NULL))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pEntity->GetID().Value());
        }
        else if (CResource* pResource = UserDataCast((CResource*)userdata, NULL))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pResource->GetScriptID());
        }
        else if (CXMLNode* pNode = UserDataCast((CXMLNode*)userdata, NULL))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pNode->GetID());
        }
        else if (CLuaTimer* pTimer = UserDataCast((CLuaTimer*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pTimer->GetScriptID());
        }
        else if (CAccount* pAccount = UserDataCast((CAccount*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pAccount->GetScriptID());
        }
        else if (CAccessControlList* pACL = UserDataCast((CAccessControlList*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pACL->GetScriptID());
        }
        else if (CAccessControlListGroup* pACLGroup = UserDataCast((CAccessControlListGroup*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pACLGroup->GetScriptID());
        }
        else if (CBan* pBan = UserDataCast((CBan*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pBan->GetScriptID());
        }
        else if (CTextDisplay* pTextDisplay = UserDataCast((CTextDisplay*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pTextDisplay->GetScriptID());
        }
        else if (CTextItem* pTextItem = UserDataCast((CTextItem*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pTextItem->GetScriptID());
        }
        else if (CDbJobData* pQuery = UserDataCast((CDbJobData*)userdata, luaVM))
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(pQuery->GetId());
        }
        else
        {
            m_pResults->data[0].of.i32 = (CWebAssemblyUserData)userdata;
        }

        return CreateTrap(errorMessage);
    }

    /*CWebAssemblyTrap* ReturnElement(CElement* element, SString errorMessage = "")
    {
        if (!element || element->IsBeingDeleted())
        {
            m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
            m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

            return CreateTrap(errorMessage);
        }

        ElementID eId = element->GetID();

        if (eId == INVALID_ELEMENT_ID)
        {
            m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
            m_pResults->data[0].of.i32 = WEB_ASSEMBLY_NULL_PTR;

            return CreateTrap(errorMessage);
        }

        m_pResults->data[0].kind = C_WASM_VARIABLE_TYPE_I32;
        m_pResults->data[0].of.i32 = (CWebAssemblyUserData)(void*)reinterpret_cast<unsigned int*>(eId.Value());

        return CreateTrap(errorMessage);
    }*/ 

    bool Skip(int count = 1);
    void Reset();

    bool CanContinue();

    CWebAssemblyTrap* CreateTrap(const SString& errorMessage);

    void                SetScript(CWebAssemblyScript* script);
    CWebAssemblyScript* GetScript();

    void    SetFunctionName(const SString& functionName);
    SString GetFunctionName();

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
    SString             m_strFunctionName;

    SString m_strError;

    int m_iIndex;

    CWebAssemblyValueVector* m_pArgs;
    CWebAssemblyValueVector* m_pResults;
};

#endif
