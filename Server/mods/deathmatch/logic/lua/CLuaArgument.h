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
    void SetFunctionResource(CResource* resource);
    void SetFunctionReference(int functionRef);

    bool               GetBoolean() const { return m_bBoolean; };
    lua_Number         GetNumber() const { return m_Number; };
    const std::string& GetString() { return m_strString; };
    void*              GetUserData() const { return m_pUserData; };
    CResource*         GetFunctionResource() const { return m_pResource; };
    int                GetFunctionReference() const { return m_iFunctionRef; };
    CElement*          GetElement() const;
    bool               GetAsString(SString& strBuffer);

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
    CResource*     m_pResource;
    int            m_iFunctionRef;
    bool           m_bWeakTableRef;

#ifdef GninE_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
