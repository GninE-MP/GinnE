/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArguments.cpp
 *  PURPOSE:     Lua argument manager class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaArguments.h"
#include "CLuaMain.h"
#include "CGame.h"
#include "CScriptDebugging.h"
#include "CPerfStatManager.h"
#include "CDatabaseManager.h"
#include "CBan.h"
#include "CAccount.h"
#include "CAccessControlList.h"
#include "CAccessControlListGroup.h"

#include "wasm/CWebAssemblyVariable.h"
#include "wasm/CWebAssemblyContext.h"

#ifndef WIN32
#include <clocale>
#endif

extern CGame* g_pGame;

#ifndef VERIFY_ELEMENT
#define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement()->IsMyChild(element, true) && !element->IsBeingDeleted())
#endif

CLuaArguments::CLuaArguments(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables)
{
    // Copy all the arguments
    CopyRecursive(Arguments, pKnownTables);
}

CLuaArgument* CLuaArguments::operator[](const unsigned int uiPosition) const
{
    if (uiPosition < m_Arguments.size())
        return m_Arguments.at(uiPosition);
    return NULL;
}

// Slow if used with a constructor as it does a copy twice
const CLuaArguments& CLuaArguments::operator=(const CLuaArguments& Arguments)
{
    CopyRecursive(Arguments);

    // Return the given reference allowing for chaining
    return Arguments;
}

void CLuaArguments::CopyRecursive(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables)
{
    // Clear our previous list if any
    DeleteArguments();

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair((CLuaArguments*)&Arguments, (CLuaArguments*)this));

    // Copy all the arguments
    std::vector<CLuaArgument*>::const_iterator iter = Arguments.m_Arguments.begin();
    for (; iter != Arguments.m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = new CLuaArgument(**iter, pKnownTables);
        m_Arguments.push_back(pArgument);
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArguments(lua_State* luaVM, signed int uiIndexBegin)
{
    // Delete the previous arguments if any
    DeleteArguments();

    CFastHashMap<const void*, CLuaArguments*> knownTables;

    // Start reading arguments until there are none left
    while (lua_type(luaVM, uiIndexBegin) != LUA_TNONE)
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument(luaVM, uiIndexBegin++, &knownTables);
        m_Arguments.push_back(pArgument);

        knownTables.clear();
    }
}

void CLuaArguments::ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables)
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<const void*, CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair(lua_topointer(luaVM, iIndexBegin), this));

    // Delete the previous arguments if any
    DeleteArguments();

    LUA_CHECKSTACK(luaVM, 2);
    lua_pushnil(luaVM); /* first key */
    if (iIndexBegin < 0)
        iIndexBegin--;

    while (lua_next(luaVM, iIndexBegin) != 0)
    {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        CLuaArgument* pArgument = new CLuaArgument(luaVM, -2, pKnownTables);
        m_Arguments.push_back(pArgument);            // push the key first

        pArgument = new CLuaArgument(luaVM, -1, pKnownTables);
        m_Arguments.push_back(pArgument);            // then the value

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArgument(lua_State* luaVM, int iIndex)
{
    CLuaArgument* pArgument = new CLuaArgument(luaVM, iIndex);
    m_Arguments.push_back(pArgument);
}

void CLuaArguments::PushArguments(lua_State* luaVM) const
{
    // Push all our arguments
    std::vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        (*iter)->Push(luaVM);
    }
}

void CLuaArguments::PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables) const
{
    // Ensure there is enough space on the Lua stack
    LUA_CHECKSTACK(luaVM, 4);

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, int>();
        bKnownTablesCreated = true;

        lua_newtable(luaVM);
        // using registry to make it fail safe, else we'd have to carry
        // either lua top or current depth variable between calls
        lua_setfield(luaVM, LUA_REGISTRYINDEX, "cache");
    }

    lua_newtable(luaVM);

    // push it onto the known tables
    int size = pKnownTables->size();
    lua_getfield(luaVM, LUA_REGISTRYINDEX, "cache");
    lua_pushnumber(luaVM, ++size);
    lua_pushvalue(luaVM, -3);
    lua_settable(luaVM, -3);
    lua_pop(luaVM, 1);
    pKnownTables->insert(std::make_pair((CLuaArguments*)this, size));

    std::vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end() && (iter + 1) != m_Arguments.end(); ++iter)
    {
        (*iter)->Push(luaVM, pKnownTables);            // index
        ++iter;
        (*iter)->Push(luaVM, pKnownTables);            // value
        lua_settable(luaVM, -3);
    }

    if (bKnownTablesCreated)
    {
        // clear the cache
        lua_pushnil(luaVM);
        lua_setfield(luaVM, LUA_REGISTRYINDEX, "cache");
        delete pKnownTables;
    }
}

void CLuaArguments::PushArguments(const CLuaArguments& Arguments)
{
    std::vector<CLuaArgument*>::const_iterator iter = Arguments.IterBegin();
    for (; iter != Arguments.IterEnd(); ++iter)
    {
        CLuaArgument* pArgument = new CLuaArgument(**iter);
        m_Arguments.push_back(pArgument);
    }
}

bool CLuaArguments::Call(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues) const
{
    assert(pLuaMain);
    TIMEUS startTime = GetTimeUs();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine();
    assert(luaVM);
    LUA_CHECKSTACK(luaVM, 1);
    int luaStackPointer = lua_gettop(luaVM);
    lua_getref(luaVM, iLuaFunction.ToInt());

    // Push our arguments onto the stack
    PushArguments(luaVM);

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount();

    int iret = pLuaMain->PCall(luaVM, m_Arguments.size(), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        SString strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                returnValues->ReadArgument(luaVM, i);
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, pLuaMain->GetFunctionTag(iLuaFunction.ToInt()), GetTimeUs() - startTime);
    return true;
}

bool CLuaArguments::CallGlobal(CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues) const
{
    assert(pLuaMain);
    assert(szFunction);
    TIMEUS startTime = GetTimeUs();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine();
    assert(luaVM);
    LUA_CHECKSTACK(luaVM, 1);
    int luaStackPointer = lua_gettop(luaVM);
    lua_pushstring(luaVM, szFunction);
    lua_gettable(luaVM, LUA_GLOBALSINDEX);

    // If that function doesn't exist, return false
    if (lua_isnil(luaVM, -1))
    {
        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;
    }

    // Push our arguments onto the stack
    PushArguments(luaVM);

    // Reset function call timer (checks long-running functions)
    pLuaMain->ResetInstructionCount();

    // Call the function with our arguments
    int iret = pLuaMain->PCall(luaVM, m_Arguments.size(), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        std::string strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                returnValues->ReadArgument(luaVM, i);
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, szFunction, GetTimeUs() - startTime);
    return true;
}

CLuaArgument* CLuaArguments::PushNil()
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBoolean(bool bBool)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadBool(bBool);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTable(CLuaArguments* table)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadTable(table);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushNumber(double dNumber)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadNumber(dNumber);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushArgument(const CLuaArgument& argument)
{
    CLuaArgument* pArgument = new CLuaArgument(argument);            // create a copy
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushString(const std::string& strString)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadString(strString);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushElement(CElement* pElement)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadElement(pElement);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBan(CBan* pBan)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pBan->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACL(CAccessControlList* pACL)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACL->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACLGroup(CAccessControlListGroup* pACLGroup)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACLGroup->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushAccount(CAccount* pAccount)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pAccount->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushResource(CResource* pResource)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pResource->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextDisplay(CTextDisplay* pTextDisplay)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextDisplay->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextItem(CTextItem* pTextItem)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextItem->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTimer(CLuaTimer* pLuaTimer)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pLuaTimer->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushDbQuery(CDbJobData* pJobData)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pJobData->GetId());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

void CLuaArguments::DeleteArguments()
{
    // Delete each item
    vector<CLuaArgument*>::iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        delete *iter;
    }

    // Clear the vector
    m_Arguments.clear();
}

// Gets rid of the last argument in the list
void CLuaArguments::Pop()
{
    // Delete the last element
    CLuaArgument* item = m_Arguments.back();
    delete item;

    // Pop it out of the vector
    m_Arguments.pop_back();
}

void CLuaArguments::ValidateTableKeys()
{
    // Iterate over m_Arguments as pairs
    // If first is LUA_TNIL, then remove pair
    vector<CLuaArgument*>::iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end();)
    {
        // Check first in pair
        if ((*iter)->GetType() == LUA_TNIL)
        {
            // TODO - Handle ref in KnownTables
            // Remove pair
            delete *iter;
            iter = m_Arguments.erase(iter);
            if (iter != m_Arguments.end())
            {
                delete *iter;
                iter = m_Arguments.erase(iter);
            }
            // Check if end
            if (iter == m_Arguments.end())
                break;
        }
        else
        {
            // Skip second in pair
            ++iter;
            // Check if end
            if (iter == m_Arguments.end())
                break;

            ++iter;
        }
    }
}

bool CLuaArguments::ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables)
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    unsigned int uiNumArgs;
    if (bitStream.ReadCompressed(uiNumArgs))
    {
        pKnownTables->push_back(this);
        for (unsigned int ui = 0; ui < uiNumArgs; ++ui)
        {
            CLuaArgument* pArgument = new CLuaArgument();
            if (!pArgument->ReadFromBitStream(bitStream, pKnownTables))
            {
                delete pArgument;
                if (bKnownTablesCreated)
                    delete pKnownTables;
                return false;
            }
            m_Arguments.push_back(pArgument);
        }
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return true;
}

bool CLuaArguments::WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables) const
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, unsigned long>();
        bKnownTablesCreated = true;
    }

    bool bSuccess = true;
    pKnownTables->insert(make_pair((CLuaArguments*)this, pKnownTables->size()));
    bitStream.WriteCompressed(static_cast<unsigned int>(m_Arguments.size()));

    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = *iter;
        if (!pArgument->WriteToBitStream(bitStream, pKnownTables))
        {
            bSuccess = false;
        }
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::WriteToJSONString(std::string& strJSON, bool bSerialize, int flags)
{
    json_object* my_array = WriteToJSONArray(bSerialize);
    if (my_array)
    {
        strJSON = json_object_to_json_string_ext(my_array, flags);
        json_object_put(my_array);            // dereference - causes a crash, is actually commented out in the example too
        return true;
    }
    return false;
}

json_object* CLuaArguments::WriteToJSONArray(bool bSerialize)
{
    json_object*                          my_array = json_object_new_array();
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = *iter;
        json_object*  object = pArgument->WriteToJSONObject(bSerialize);
        if (object)
        {
            json_object_array_add(my_array, object);
        }
        else
        {
            break;
        }
    }
    return my_array;
}

json_object* CLuaArguments::WriteTableToJSONObject(bool bSerialize, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables)
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, unsigned long>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair(this, pKnownTables->size()));

    bool                                  bIsArray = true;
    unsigned int                          iArrayPos = 1;            // lua arrays are 1 based
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter += 2)
    {
        CLuaArgument* pArgument = *iter;
        if (pArgument->GetType() == LUA_TNUMBER)
        {
            double       num = pArgument->GetNumber();
            unsigned int iNum = static_cast<unsigned int>(num);
            if (num == iNum)
            {
                if (iArrayPos != iNum)            // check if the value matches its index in the table
                {
                    bIsArray = false;
                    break;
                }
            }
            else
            {
                bIsArray = false;
                break;
            }
        }
        else
        {
            bIsArray = false;
            break;
        }
        iArrayPos++;
    }

    if (bIsArray)
    {
        json_object*                          my_array = json_object_new_array();
        vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
        for (; iter != m_Arguments.end(); ++iter)
        {
            iter++;            // skip the key values
            CLuaArgument* pArgument = *iter;
            json_object*  object = pArgument->WriteToJSONObject(bSerialize, pKnownTables);
            if (object)
            {
                json_object_array_add(my_array, object);
            }
            else
            {
                break;
            }
        }
        if (bKnownTablesCreated)
            delete pKnownTables;
        return my_array;
    }
    else
    {
        json_object* my_object = json_object_new_object();
        iter = m_Arguments.begin();
        for (; iter != m_Arguments.end(); ++iter)
        {
            char szKey[255];
            szKey[0] = '\0';
            CLuaArgument* pArgument = *iter;
            if (!pArgument->WriteToString(szKey, 255))            // index
                break;
            ++iter;
            pArgument = *iter;
            json_object* object = pArgument->WriteToJSONObject(bSerialize, pKnownTables);            // value

            if (object)
            {
                json_object_object_add(my_object, szKey, object);
            }
            else
            {
                break;
            }
        }
        if (bKnownTablesCreated)
            delete pKnownTables;
        return my_object;
    }
}

bool CLuaArguments::ReadFromJSONString(const char* szJSON)
{
    // Fast isJSON check: Check first non-white space character is '[' or '{'
    for (const char* ptr = szJSON; true;)
    {
        char c = *ptr++;
        if (c == '[' || c == '{')
            break;
        if (isspace((uchar)c))
            continue;
        return false;
    }

    json_object* object = json_tokener_parse(szJSON);
    if (object)
    {
        if (json_object_get_type(object) == json_type_array)
        {
            bool bSuccess = true;

            std::vector<CLuaArguments*> knownTables;

            for (uint i = 0; i < json_object_array_length(object); i++)
            {
                json_object*  arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument* pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(arrayObject, &knownTables);
                m_Arguments.push_back(pArgument);            // then the value
                if (!bSuccess)
                    break;
            }
            json_object_put(object);            // dereference
            return bSuccess;
        }
        else if (json_object_get_type(object) == json_type_object)
        {
            std::vector<CLuaArguments*> knownTables;
            CLuaArgument*               pArgument = new CLuaArgument();
            bool                        bSuccess = pArgument->ReadFromJSONObject(object, &knownTables);
            m_Arguments.push_back(pArgument);            // value
            json_object_put(object);

            return bSuccess;
        }
        json_object_put(object);            // dereference
    }
    //    else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    //   else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse HTTP POST request, ensure data uses JSON.");
    return false;
}

bool CLuaArguments::ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables)
{
    if (object)
    {
        if (json_object_get_type(object) == json_type_object)
        {
            bool bKnownTablesCreated = false;
            if (!pKnownTables)
            {
                pKnownTables = new std::vector<CLuaArguments*>();
                bKnownTablesCreated = true;
            }

            pKnownTables->push_back(this);

            bool bSuccess = true;
            json_object_object_foreach(object, key, val)
            {
                CLuaArgument* pArgument = new CLuaArgument();
                pArgument->ReadString(key);
                m_Arguments.push_back(pArgument);            // push the key first
                pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(val, pKnownTables);            // then the value
                m_Arguments.push_back(pArgument);
                if (!bSuccess)
                    break;
            }

            if (bKnownTablesCreated)
                delete pKnownTables;
            return bSuccess;
        }
    }
    //   else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}

bool CLuaArguments::ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* pKnownTables)
{
    if (object)
    {
        if (json_object_get_type(object) == json_type_array)
        {
            bool bKnownTablesCreated = false;
            if (!pKnownTables)
            {
                pKnownTables = new std::vector<CLuaArguments*>();
                bKnownTablesCreated = true;
            }

            pKnownTables->push_back(this);

            bool bSuccess = true;
            for (uint i = 0; i < json_object_array_length(object); i++)
            {
                json_object*  arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument* pArgument = new CLuaArgument();
                pArgument->ReadNumber(i + 1);            // push the key
                m_Arguments.push_back(pArgument);

                pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(arrayObject, pKnownTables);
                m_Arguments.push_back(pArgument);            // then the valoue
                if (!bSuccess)
                    break;
            }

            if (bKnownTablesCreated)
                delete pKnownTables;
            return bSuccess;
        }
    }
    //    else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}

void CLuaArguments::WriteWebAssemblyVariables(CWebAssemblyVariables* vars, CWebAssemblyVariables* typePattern, CWebAssemblyMemory* memory, std::vector<CWebAssemblyMemoryPointerAddress>* pointersToDelete)
{
    if (!vars)
    {
        return;
    }

    int argsSize = m_Arguments.size();
    int typePatternSize = typePattern ? typePattern->GetSize() : argsSize;

    for (int i = 0; i < argsSize && i < typePatternSize; i++)
    {
        CLuaArgument* arg = m_Arguments[i];

        CWebAssemblyVariable pattern = typePattern ? typePattern->Get(i) : CWebAssemblyVariable((int32_t)0);

        CWebAssemblyValue value = pattern.GetValue();

        CWebAssemblyMemoryPointerAddress ptr = WEB_ASSEMBLY_NULL_PTR;

        CCallable callable = arg->GetCallable();

        #define SET_VALUE(val) \
            if (typePattern) \
            { \
                switch (value.kind) \
                { \
                    case C_WASM_VARIABLE_TYPE_I32: \
                        value.of.i32 = (int32_t)val; \
                        break; \
                    case C_WASM_VARIABLE_TYPE_I64: \
                        value.of.i64 = (int64_t)val; \
                        break; \
                    case C_WASM_VARIABLE_TYPE_F32: \
                        value.of.f32 = (float32_t)val; \
                        break; \
                    case C_WASM_VARIABLE_TYPE_F64: \
                        value.of.f64 = (float64_t)val; \
                        break; \
                    default: \
                        break; \
                } \
                vars->Push(value); \
            } \
            else \

        switch (arg->GetType())
        {
            case LUA_TNIL:
                SET_VALUE(WEB_ASSEMBLY_NULL_PTR)
                {
                    vars->Push((int32_t)WEB_ASSEMBLY_NULL_PTR);
                }

                break;
            case LUA_TBOOLEAN:
                SET_VALUE(arg->GetBoolean())
                {
                    vars->Push((int32_t)arg->GetBoolean());
                }

                break;
            case LUA_TNUMBER:
                SET_VALUE(arg->GetNumber())
                {
                    vars->Push((float64_t)arg->GetNumber());
                }

                break;
            case LUA_TSTRING:
                if (memory)
                {
                    ptr = memory->StringToUTF8(arg->GetString());

                    vars->Push((int32_t)ptr);
                }

                break;
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
                vars->Push((CWebAssemblyUserData)arg->GetUserData());

                break;
            case LUA_TTABLE:
                break;
            case LUA_TFUNCTION:
                if (memory)
                {
                    void* pPtr = NULL;
                    ptr = memory->Malloc(C_CALLABLE_HASH_SIZE, &pPtr);

                    if (pPtr)
                    {
                        callable.WriteHash((uint8_t*)pPtr);
                        vars->Push((int32_t)ptr);
                    }
                }

                break;
            default:
                break;
        }

        #undef SET_VALUE

        if (pointersToDelete && ptr != WEB_ASSEMBLY_NULL_PTR)
        {
            pointersToDelete->push_back(ptr);
        }
    }
}

void CLuaArguments::ReadWebAssemblyVariables(CWebAssemblyVariables* vars)
{
    if (!vars)
    {
        return;
    }

    for (CWebAssemblyVariable& var : vars->GetList())
    {
        CLuaArgument* arg = new CLuaArgument();

        switch (var.GetType())
        {
            case C_WASM_VARIABLE_TYPE_I32:
                arg->SetNumber((lua_Number)var.GetInt32());
                break;
            case C_WASM_VARIABLE_TYPE_I64:
                arg->SetNumber((lua_Number)var.GetInt64());
                break;
            case C_WASM_VARIABLE_TYPE_F32:
                arg->SetNumber((lua_Number)var.GetFloat32());
                break;
            case C_WASM_VARIABLE_TYPE_F64:
                arg->SetNumber((lua_Number)var.GetFloat64());
                break;
            default:
                break;
        }

        m_Arguments.push_back(arg);
    }
}

void CLuaArguments::UnpackCompiledArguments(uint8_t* data, size_t size, bool readAsTable)
{
    if (!data || size < C_ARGS_SEPRATOR_LENGTH)
    {
        return;
    }

    int  openedList = 0;
    bool isInsideString = false;

    for (long long i = 0, lastPos = 0, tkey = 1; i < size; i++)
    {
        char* pos = (char*)data + i;

        if (isInsideString)
        {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0)
            {
                if (strncmp(pos - 1, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0)
                {
                    continue;
                }

                isInsideString = false;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0)
        {
            isInsideString = true;
            continue;
        }

        if (openedList > 0)
        {
            if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0)
            {
                openedList++;
            }
            else if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0)
            {
                openedList--;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0)
        {
            openedList++;
        }

        if (strncmp(pos, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH) == 0)
        {
            long long size = i - lastPos;
            char*     argData = pos - (intptr_t)size;

            if (lastPos > 0)
            {
                argData += C_ARGS_SEPRATOR_LENGTH;
                size -= C_ARGS_SEPRATOR_LENGTH;
            }

            char argType = *argData;

            argData += C_ARGS_TYPE_IDENTIFIER_LENGTH;
            size -= C_ARGS_TYPE_IDENTIFIER_LENGTH;

            CLuaArgument* pArgument = new CLuaArgument();

            switch (argType)
            {
                case C_ARGS_TYPE_ID_UNKNOWN:
                case C_ARGS_TYPE_ID_NULL:
                    break;
                case C_ARGS_TYPE_ID_BOOLEAN:
                    pArgument->SetBoolean(*argData == '1' ? true : false);
                    break;
                case C_ARGS_TYPE_ID_NUMBER:
                    pArgument->SetNumber(strtod(argData, NULL));
                    break;
                case C_ARGS_TYPE_ID_STRING:
                    {
                        argData += C_ARGS_STRING_SYNTAX_LENGTH;
                        size -= C_ARGS_STRING_SYNTAX_LENGTH * 2;

                        char* str = (char*)malloc(size + 1);
                        memset((void*)str, 0, size + 1);

                        size_t sSize = size;
                        for (size_t i = 0, ci = 0; i < sSize; i++) {
                            char* chunk = argData + i;

                            if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                                if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                                    size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                                    str = (char*)realloc((void*)str, size);

                                    continue;
                                }
                            }
        
                            str[ci] = *chunk;
                            ci++;
                        }

                        str[size] = '\0';

                        pArgument->SetString(std::string(str, size));

                        free((void*)str);
                    }

                    break;
                case C_ARGS_TYPE_ID_CALLABLE:
                    {
                        argData += C_ARGS_STRING_SYNTAX_LENGTH;
                        size -= C_ARGS_STRING_SYNTAX_LENGTH * 2;

                        char* str = (char*)malloc(size + 1);
                        memset((void*)str, 0, size + 1);

                        size_t sSize = size;
                        for (size_t i = 0, ci = 0; i < sSize; i++) {
                            char* chunk = argData + i;

                            if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                                if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                                    size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                                    str = (char*)realloc((void*)str, size);

                                    continue;
                                }
                            }
        
                            str[ci] = *chunk;
                            ci++;
                        }

                        CCallable callable;

                        if (callable.ReadHash((uint8_t*)str))
                        {
                            pArgument->SetCallable(callable);
                        }

                        free((void*)str);
                    }

                    break;
                case C_ARGS_TYPE_ID_LIST:
                    {
                        while (strncmp(argData, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) != 0 && size > 0)
                        {
                            argData++;
                            size--;
                        }

                        argData += C_ARGS_LIST_OPEN_SYNTAX_LENGTH;
                        size -= C_ARGS_LIST_OPEN_SYNTAX_LENGTH + C_ARGS_LIST_CLOSE_SYNTAX_LENGTH;

                        if (size > 0)
                        {
                            CLuaArguments* tData = new CLuaArguments();

                            if (tData)
                            {
                                tData->UnpackCompiledArguments((uint8_t*)argData, size, true);

                                pArgument->ReadTable(tData);

                                delete tData;
                            }
                        }
                    }

                    break;
                case C_ARGS_TYPE_ID_USERDATA:
                    #if IS_APP_ON_64_BIT_VERSION
                        pArgument->SetUserData((void*)strtoll(argData, NULL, 10));
                    #else
                        pArgument->SetUserData((void*)atoi(argData));
                    #endif
                    break;
                default:
                    break;
            }

            if (pArgument)
            {
                if (readAsTable)
                {
                    CLuaArgument* tableKey = new CLuaArgument();
                    tableKey->SetNumber(tkey);

                    m_Arguments.push_back(tableKey);

                    tkey++;
                }

                m_Arguments.push_back(pArgument);
            }

            lastPos = i;
        }
    }
}

bool CLuaArguments::IsEqualTo(const CLuaArguments& compareTo, std::set<const CLuaArguments*>* knownTables) const
{
    if (m_Arguments.size() != compareTo.m_Arguments.size())
        return false;

    if (knownTables != nullptr)
    {
        if (knownTables->find(&compareTo) != knownTables->end())
            return true;

        knownTables->insert(&compareTo);
    }

    return std::equal(std::begin(m_Arguments), std::end(m_Arguments), std::begin(compareTo.m_Arguments),
                      [knownTables](const CLuaArgument* lhs, const CLuaArgument* rhs) { return lhs->IsEqualTo(*rhs, knownTables); });
}
