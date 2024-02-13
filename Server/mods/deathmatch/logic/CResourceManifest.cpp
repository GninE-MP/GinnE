/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceManifest.cpp
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "lua/CLuaMain.h"

#include "CResourceManifest.h"
#include "CResourceScriptItem.h"
#include "CResourceClientFileItem.h"
#include "CResourceClientScriptItem.h"

#include "Utils.h"

#define RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME "RESOURCE_MAIN_MANIFEST_OBJECT"

CResourceManifest::CResourceManifest(CResource* resource, const SString& manifestFilePath)
{
    m_strResourceManifestFilePath = manifestFilePath;
    ConvertFileNameToWindowsFormat(m_strResourceManifestFilePath);

    m_pResource = resource;
    m_pLuaVM = NULL;
}

CResourceManifest::~CResourceManifest()
{
    Unload();
}

void CResourceManifest::Load()
{
    if (m_strResourceManifestFilePath.empty())
    {
        return;
    }

    std::string manifestPath;
    if (!m_pResource->GetFilePath(m_strResourceManifestFilePath.c_str(), manifestPath))
    {
        CLogger::ErrorPrintf("Couldn't find manifest file[%s].", m_strResourceManifestFilePath.c_str());
        return;
    }

    FILE* manifestFile = File::Fopen(manifestPath.c_str(), "rb");

    if (!manifestFile)
    {
        CLogger::ErrorPrintf("Couldn' load resource manifest file[%s].\n", m_strResourceManifestFilePath.c_str());
        return;
    }

    fclose(manifestFile);

    m_pLuaVM = lua_open(m_pResource->GetVirtualMachine());

    luaopen_base(m_pLuaVM);
    luaopen_debug(m_pLuaVM);
    luaopen_math(m_pLuaVM);
    luaopen_os(m_pLuaVM);
    luaopen_string(m_pLuaVM);
    luaopen_table(m_pLuaVM);
    luaopen_utf8(m_pLuaVM);

    lua_pushuserdata(m_pLuaVM, this);
    lua_setglobal(m_pLuaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    RegisterLuaVMFunctions(m_pLuaVM);

    if (luaL_dofile(m_pLuaVM, manifestPath.c_str()))
    {
        const char* error = lua_tostring(m_pLuaVM, -1);

        CLogger::ErrorPrintf("[MANIFEST] %s\n", error);

        lua_pop(m_pLuaVM, 1);
    }

    lua_close(m_pLuaVM);
}

void CResourceManifest::Unload()
{
    std::list<CResourceFile*>& resourceFiles = m_pResource->GetFiles();

    for (CResourceFile* file : m_pResourceFiles)
    {
        resourceFiles.remove(file);

        if (file->IsNoClientCache())
        {
            FileDelete(
                PathJoin(
                    g_pServerInterface->GetServerModPath(),
                    "resource-cache",
                    "http-client-files-no-client-cache",
                    m_pResource->GetName(),
                    file->GetName()
                )
            );
        }
    }

    m_pResourceFiles.clear();
}

SString& CResourceManifest::GetManifestPath()
{
    return m_strResourceManifestFilePath;
}

SString CResourceManifest::GetManifestAbsolutePath()
{
    std::string path;

    if (!m_pResource->GetFilePath(m_strResourceManifestFilePath.c_str(), path))
    {
        return "";
    }

    return path;
}

CResource* CResourceManifest::GetResource()
{
    return m_pResource;
}

std::list<CResourceFile*>& CResourceManifest::GetResourceFiles()
{
    return m_pResourceFiles;
}

void CResourceManifest::RegisterLuaVMFunctions(lua_State* luaVM)
{
    lua_register(luaVM, "client_scripts", Lua_ClientScripts);
    lua_register(luaVM, "client_script", Lua_ClientScripts);
    lua_register(luaVM, "server_scripts", Lua_ServerScripts);
    lua_register(luaVM, "server_script", Lua_ServerScripts);
    lua_register(luaVM, "files", Lua_Files);
    lua_register(luaVM, "file", Lua_Files);
}

void CResourceManifest::ConvertFileNameToWindowsFormat(SString& fileName)
{
    size_t length = fileName.size();

    for (size_t i = 0; i < length; i++)
    {
        if (fileName[i] == '/')
        {
            fileName[i] = '\\';
        }
    }
}

void CResourceManifest::AddClientScript(CResourceManifest* manifest, const char* fileName)
{
    CXMLNode*       node = g_pServerInterface->GetXML()->CreateDummyNode();
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* cacheAttribute = attributes.Create("cache");
    cacheAttribute->SetValue("false");

    CResource* pResource = manifest->GetResource();

    std::string fileNameFixed = fileName;
    ReplaceOccurrencesInString(fileNameFixed, "\\", "/");

    if (!pResource->IsFilenameUsed(fileNameFixed.c_str(), true))
    {
        SString fileFullPath;
        if (IsValidFilePath(fileNameFixed.c_str()) && pResource->GetFilePath(fileNameFixed.c_str(), fileFullPath))
        {
            CResourceClientScriptItem* scriptItem = new CResourceClientScriptItem(pResource, fileNameFixed.c_str(), fileFullPath.c_str(), &attributes);

            SString buffer;
            FileLoad(fileFullPath.c_str(), buffer);

            if (!buffer.empty())
            {
                FileSave(
                    PathJoin(
                        g_pServerInterface->GetServerModPath(),
                        "resource-cache",
                        "http-client-files-no-client-cache",
                        pResource->GetName(),
                        fileNameFixed
                    ),
                    buffer,
                    true
                );
            }

            manifest->GetResourceFiles().push_back(scriptItem);
            pResource->GetFiles().push_back(scriptItem);

            pResource->GenerateChecksumForFile(scriptItem).get();
        }
    }

    delete node;
}

void CResourceManifest::AddServerScript(CResourceManifest* manifest, const char* fileName)
{
    CXMLNode*       node = g_pServerInterface->GetXML()->CreateDummyNode();
    CXMLAttributes& attributes = node->GetAttributes();

    CResource* pResource = manifest->GetResource();

    std::string fileNameFixed = fileName;
    ReplaceOccurrencesInString(fileNameFixed, "\\", "/");

    if (!pResource->IsFilenameUsed(fileNameFixed.c_str(), false))
    {
        SString fileFullPath;
        if (IsValidFilePath(fileNameFixed.c_str()) && pResource->GetFilePath(fileNameFixed.c_str(), fileFullPath))
        {
            CResourceScriptItem* scriptItem = new CResourceScriptItem(pResource, fileNameFixed.c_str(), fileFullPath.c_str(), &attributes);

            manifest->GetResourceFiles().push_back(scriptItem);
            pResource->GetFiles().push_back(scriptItem);

            pResource->GenerateChecksumForFile(scriptItem).get();
        }
    }

    delete node;
}

void CResourceManifest::AddFile(CResourceManifest* manifest, const char* fileName)
{
    // here for this file mismatch happens everytime on client. but when client reconnects it fixes itself.
    // the problem is on restarting file client doesn't handle changes and doesn't download new file!

    CXMLNode*       node = g_pServerInterface->GetXML()->CreateDummyNode();
    CXMLAttributes& attributes = node->GetAttributes();

    CResource* pResource = manifest->GetResource();

    std::string fileNameFixed = fileName;
    ReplaceOccurrencesInString(fileNameFixed, "\\", "/");

    if (!pResource->IsFilenameUsed(fileNameFixed.c_str(), true))
    {
        SString fileFullPath;
        if (IsValidFilePath(fileNameFixed.c_str()) && pResource->GetFilePath(fileNameFixed.c_str(), fileFullPath))
        {
            CResourceClientFileItem* fileItem = new CResourceClientFileItem(pResource, fileNameFixed.c_str(), fileFullPath.c_str(), &attributes);

            manifest->GetResourceFiles().push_back(fileItem);
            pResource->GetFiles().push_back(fileItem);

            pResource->GenerateChecksumForFile(fileItem).get();
        }
    }

    delete node;
}

int CResourceManifest::Lua_ClientScripts(lua_State* luaVM)
{
    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        return 0;
    }

    if (lua_type(luaVM, -1) == LUA_TSTRING)
    {
        const char* fileName = lua_tostring(luaVM, -1);

        AddClientScript(pManifest, fileName);
    }
    else if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t len = lua_objlen(luaVM, -1);

        for (size_t i = 1; i <= len; i++)
        {
            lua_pushinteger(luaVM, i);
            lua_gettable(luaVM, -2);

            if (lua_type(luaVM, -1) == LUA_TSTRING)
            {
                const char* fileName = lua_tostring(luaVM, -1);

                AddClientScript(pManifest, fileName);
            }

            lua_pop(luaVM, 1);
        }
    }

    return 0;
}

int CResourceManifest::Lua_ServerScripts(lua_State* luaVM)
{
    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        return 0;
    }

    if (lua_type(luaVM, -1) == LUA_TSTRING)
    {
        const char* fileName = lua_tostring(luaVM, -1);

        AddServerScript(pManifest, fileName);
    }
    else if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t len = lua_objlen(luaVM, -1);

        for (size_t i = 1; i <= len; i++)
        {
            lua_pushinteger(luaVM, i);
            lua_gettable(luaVM, -2);

            if (lua_type(luaVM, -1) == LUA_TSTRING)
            {
                const char* fileName = lua_tostring(luaVM, -1);

                AddServerScript(pManifest, fileName);
            }

            lua_pop(luaVM, 1);
        }
    }

    return 0;
}

int CResourceManifest::Lua_Files(lua_State* luaVM)
{
    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        return 0;
    }

    if (lua_type(luaVM, -1) == LUA_TSTRING)
    {
        const char* fileName = lua_tostring(luaVM, -1);

        AddFile(pManifest, fileName);
    }
    else if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t len = lua_objlen(luaVM, -1);

        for (size_t i = 1; i <= len; i++)
        {
            lua_pushinteger(luaVM, i);
            lua_gettable(luaVM, -2);

            if (lua_type(luaVM, -1) == LUA_TSTRING)
            {
                const char* fileName = lua_tostring(luaVM, -1);

                AddFile(pManifest, fileName);
            }

            lua_pop(luaVM, 1);
        }
    }

    return 0;
}
