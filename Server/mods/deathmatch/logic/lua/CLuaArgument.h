/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

extern "C"
{
#include "lua.h"
}
#include "../common/CBitStream.h"
#include "json.h"

class CElement;
class CLuaArguments;
class CLuaMain;
class CResource;

#define LUA_TTABLEREF    9
#define LUA_TSTRING_LONG 10
#define LUA_CALLABLE 0xfac

#define C_CALLABLE_HASH_HEADER_BYTES "crbh1"
#define C_CALLABLE_HASH_SIZE ( \
    sizeof(C_CALLABLE_HASH_HEADER_BYTES) - 1 + \
    sizeof(CWebAssemblyUserData)*2 + \
    sizeof(uint8_t) \
)

class CWebAssemblyFunction;
class CWebAssemblyVariables;

class CCallable
{
public:
    CCallable();
    CCallable(CWebAssemblyFunction* function);
    CCallable(CResource* resource, int luaFunctionRef);
    ~CCallable() = default;

    bool Call(CLuaArguments* args, CLuaArguments* results, SString* errorMessage = NULL) const; // args! CLuaArguments

    void WriteHash(uint8_t* hash);
    bool ReadHash(uint8_t* hash);

    void Drop();

    void       SetLuaResource(CResource* resource);
    CResource* GetLuaResource() const;

    void SetLuaFunctionRef(int luaFunctionRef);
    int  GetLuaFunctionRef() const;

    void                  SetWasmFunction(CWebAssemblyFunction* function);
    CWebAssemblyFunction* GetWasmFunction() const;

    void SetIsWasmFunctionState(bool state);

    bool IsWasmFunction() const;

    bool operator==(CCallable target) const;

private:
    CResource* m_pLuaResource;
    int        m_iLuaFunctionRef;

    CWebAssemblyFunction* m_pWasmFunction;
    bool                  m_bIsWasmFunction;
};

class CLuaArgument
{
public:
    CLuaArgument();
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    ~CLuaArgument();

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument) const;
    bool                operator!=(const CLuaArgument& Argument) const;

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = NULL) const;

    void ReadBool(bool bBool);
    void ReadNumber(double dNumber);
    void ReadString(const std::string& strString);
    void ReadElement(CElement* pElement);
    void ReadElementID(ElementID ID);
    void ReadScriptID(uint uiScriptID);
    void ReadTable(class CLuaArguments* table);

    int GetType() const { return m_iType; };

    void SetBoolean(bool value);
    void SetNumber(lua_Number value);
    void SetString(std::string string);
    void SetUserData(void* userData);
    void SetCallable(CCallable callable);

    bool               GetBoolean() const { return m_bBoolean; };
    lua_Number         GetNumber() const { return m_Number; };
    const std::string& GetString() { return m_strString; };
    void*              GetUserData() const { return m_pUserData; };
    CElement*          GetElement() const;
    bool               GetAsString(SString& strBuffer);
    CCallable          GetCallable() const { return m_Callable; }

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL);
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);
    char*        WriteToString(char* szBuffer, int length);

    bool IsEqualTo(const CLuaArgument& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const;

    static int CallFunction(lua_State* luaVM);
    static int CleanupFunction(lua_State* luaVM);

private:
    void LogUnableToPacketize(const char* szMessage) const;

    int            m_iType;
    bool           m_bBoolean;
    lua_Number     m_Number;
    std::string    m_strString;
    void*          m_pUserData;
    CLuaArguments* m_pTableData;
    bool           m_bWeakTableRef;

    CCallable      m_Callable;

#ifdef GninE_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
