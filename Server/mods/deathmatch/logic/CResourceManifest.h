/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceManifest.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CResource.h"

class CResourceManifest
{
public:
    CResourceManifest(CResource* resource, const SString& manifestFilePath);
    ~CResourceManifest();

    void Load();
    void Unload();

    SString& GetManifestPath();
    SString  GetManifestAbsolutePath();

    CResource*                 GetResource();
    std::list<CResourceFile*>& GetResourceFiles();

private:
    SString m_strResourceManifestFilePath;

    CResource* m_pResource;
    lua_State* m_pLuaVM;

    std::list<CResourceFile*> m_pResourceFiles;

    void RegisterLuaVMFunctions(lua_State* luaVM);

    static void ConvertFileNameToWindowsFormat(SString& fileName);

    static void AddClientScript(CResourceManifest* manifest, const char* fileName);
    static void AddServerScript(CResourceManifest* manifest, const char* fileName);
    static void AddFile(CResourceManifest* manifest, const char* fileName);
    static void AddWasmClientScript(CResourceManifest* manifest, const char* fileName);
    static void AddWasmServerScript(CResourceManifest* manifest, const char* fileName);

    static int Lua_ClientScripts(lua_State* luaVM);
    static int Lua_ServerScripts(lua_State* luaVM);
    static int Lua_Files(lua_State* luaVM);

    static int Lua_Execute(lua_State* luaVM);
    static int Lua_PExecute(lua_State* luaVM);

    static int Lua_GetFilePathInResource(lua_State* luaVM);

    static int Lua_WasmCompilerInstalled(lua_State* luaVM);
    static int Lua_WasmCompile(lua_State* luaVM);
    static int Lua_WasmClient(lua_State* luaVM);
    static int Lua_WasmServer(lua_State* luaVM);
    static int Lua_WasmHasFileValidBinary(lua_State* luaVM);
};
