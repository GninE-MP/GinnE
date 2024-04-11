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

#include <stdio.h>
#include <stdexcept>

#include "StdInc.h"

#include "lua/CLuaMain.h"

#include "CResourceManifest.h"
#include "CResourceScriptItem.h"
#include "CResourceClientFileItem.h"
#include "CResourceClientScriptItem.h"

#include "wasm/CWebAssemblyContext.h"

#include "Utils.h"

#define RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME "RESOURCE_MAIN_MANIFEST_OBJECT"

#define MANIFEST_CURRENT_RESOURCE_GLOBAL_NAME            "__CURRENT_RESOURCE_NAME__"
#define MANIFEST_CURRENT_RESOURCE_PATH_GLOBAL_NAME       "__CURRENT_RESOURCE_PATH__"
#define MANIFEST_CURRENT_RESOURCE_CATCH_PATH_GLOBAL_NAME "__CURRENT_RESOURCE_CATCH_PATH__"
#define MANIFEST_CURRENT_FILE_NAME                       "__CURRENT_FILE_NAME__"
#define MANIFEST_CURRENT_FILE_PATH                       "__CURRENT_FILE_PATH__"

#define WASM_COMPILER_COMMAND       "emcc"
#define WASM_COMPILER_NAME          "Emscripten"
#define WASM_COMPILER_FILE_NAME     "emcc.bat"
#define WASM_COMPILER_PATH_IDENTIER "upstream\\emscripten"

#define WINDOWS_PATH_ENVIRONMENT_VARIABLE_NAME "PATH"

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
        CLogger::ErrorPrintf("Couldn't find manifest file[%s].\n", m_strResourceManifestFilePath.c_str());
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

    lua_pushstring(m_pLuaVM, m_pResource->GetName());
    lua_setglobal(m_pLuaVM, MANIFEST_CURRENT_RESOURCE_GLOBAL_NAME);

    lua_pushstring(m_pLuaVM, m_pResource->GetResourceDirectoryPath().c_str());
    lua_setglobal(m_pLuaVM, MANIFEST_CURRENT_RESOURCE_PATH_GLOBAL_NAME);

    lua_pushstring(m_pLuaVM, m_pResource->GetResourceCacheDirectoryPath().c_str());
    lua_setglobal(m_pLuaVM, MANIFEST_CURRENT_RESOURCE_CATCH_PATH_GLOBAL_NAME);

    std::vector<SString> parts;
    m_strResourceManifestFilePath.Split("\\", parts);

    lua_pushstring(m_pLuaVM, parts[parts.size() - 1].c_str());
    lua_setglobal(m_pLuaVM, MANIFEST_CURRENT_FILE_NAME);

    lua_pushstring(m_pLuaVM, manifestPath.c_str());
    lua_setglobal(m_pLuaVM, MANIFEST_CURRENT_FILE_PATH);


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

    lua_register(luaVM, "execute", Lua_Execute);
    lua_register(luaVM, "pexecute", Lua_PExecute);

    lua_register(luaVM, "get_file_path_in_resource", Lua_GetFilePathInResource);

    lua_register(luaVM, "wasm_compiler_installed", Lua_WasmCompilerInstalled);
    lua_register(luaVM, "wasm_compile", Lua_WasmCompile);
    lua_register(luaVM, "wasm_client", Lua_WasmClient);
    lua_register(luaVM, "wasm_server", Lua_WasmServer);
    lua_register(luaVM, "wasm_has_file_valid_binary", Lua_WasmHasFileValidBinary);
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

int CResourceManifest::Lua_Execute(lua_State* luaVM)
{
    if (lua_type(luaVM, -1) != LUA_TSTRING)
    {
        CLogger::ErrorPrintf("`execute` function expects `string` as first argument!\n");
        return 0;
    }

    const char* cmd = lua_tostring(luaVM, -1);

    if (strlen(cmd) < 1)
    {
        return 0;
    }

    int result = system(cmd);

    lua_pushnumber(luaVM, result);

    return 1;
}

int CResourceManifest::Lua_PExecute(lua_State* luaVM)
{
    if (lua_type(luaVM, -1) != LUA_TSTRING)
    {
        CLogger::ErrorPrintf("`pexecute` function expects `string` as first argument!\n");
        return 0;
    }

    const char* cmd = lua_tostring(luaVM, -1);

    if (strlen(cmd) < 1)
    {
        return 0;
    }

    char buffer[128];
    
    SString result = "";

    FILE* pipe = _popen(cmd, "r");

    if (!pipe)
    {
        CLogger::ErrorPrintf("Couldn't open pipe!\n");
        return 0;
    }

    try
    {
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            result += buffer;
        }
    }
    catch (std::exception error)
    {
        _pclose(pipe);

        lua_pushnil(luaVM);
        lua_pushstring(luaVM, error.what());

        return 2;
    }

    _pclose(pipe);

    lua_pushstring(luaVM, result.c_str());

    return 1;
}

int CResourceManifest::Lua_GetFilePathInResource(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) != LUA_TSTRING)
    {
        CLogger::ErrorPrintf("`get_file_path_in_resource` function expects `string` as first argument\n");
        return 0;
    }

    bool checkIfExists = true;

    if (lua_type(luaVM, 2) == LUA_TBOOLEAN)
    {
        checkIfExists = lua_toboolean(luaVM, 2);
    }

    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        return 0;
    }

    CResource* resource = pManifest->GetResource();

    const char* fileName = lua_tostring(luaVM, 1);

    SString path = "";

    bool exists = resource->GetFilePath(fileName, path);

    if (checkIfExists && !exists)
    {
        return 0;
    }

    ConvertFileNameToWindowsFormat(path);

    lua_pushstring(luaVM, path.c_str());

    return 1;
}

int CResourceManifest::Lua_WasmCompilerInstalled(lua_State* luaVM)
{
    const char* contentData = getenv(WINDOWS_PATH_ENVIRONMENT_VARIABLE_NAME);

    if (contentData == NULL)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    SString content = contentData;

    std::vector<SString> pathes;
    content.Split(";", pathes);

    for (SString path : pathes)
    {
        if (path.find(WASM_COMPILER_PATH_IDENTIER, 0) != SString::npos)
        {
            SString compilerPath = path;
            compilerPath += '\\';
            compilerPath += WASM_COMPILER_FILE_NAME;

            if (FileExists(compilerPath.c_str()))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }

    CLogger::LogPrintf("GNINE uses '%s' compiler to compile wasm modules, but it seems like we can't find this compiler on your operating system! You can make your own compiler using `execute` or `pexecute` functions in resource manifest file.\n", WASM_COMPILER_NAME);

    lua_pushboolean(luaVM, false);

    return 1;
}

int CResourceManifest::Lua_WasmCompile(lua_State* luaVM)
{
    return 0;
}

int CResourceManifest::Lua_WasmClient(lua_State* luaVM)
{
    return 0;
}

int CResourceManifest::Lua_WasmServer(lua_State* luaVM)
{
    if (lua_type(luaVM, -1) != LUA_TSTRING)
    {
        CLogger::ErrorPrintf("`wasm_server` function expects a `string` as first argument.\n");
        return 0;
    }

    SString filePath = lua_tostring(luaVM, -1);

    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        CLogger::ErrorPrintf("Manifest confusion happend!\n");
        return 0;
    }

    CResource* pResource = pManifest->GetResource();

    SString path = "";

    bool exists = pResource->GetFilePath(filePath.c_str(), path);

    if (!exists)
    {
        CLogger::ErrorPrintf("File doesn't exist in resource[%s].\n", filePath.c_str());
        return 0;
    }

    // create `CResourceWasmScriptItem` class!

    FILE* wasmFile = File::Fopen(path.c_str(), "rb");

    if (!wasmFile)
    {
        CLogger::ErrorPrintf("Couldn't load file[%s].\n", filePath.c_str());
        return 0;
    }

    fseek(wasmFile, 0, SEEK_END);
    size_t bufferSize = ftell(wasmFile);
    fseek(wasmFile, 0, SEEK_SET);

    char* wasmBinary = (char*)malloc(bufferSize);

    if (fread(wasmBinary, bufferSize, 1, wasmFile) != 1)
    {
        CLogger::ErrorPrintf("Couldn't read file[%s].\n", filePath.c_str());

        fclose(wasmFile);

        free(wasmBinary);

        return 0;
    }

    fclose(wasmFile);

    CWebAssemblyContext wasmModule(pResource);

    CWebAssemblyLoadState state = wasmModule.LoadScriptBinary(wasmBinary, bufferSize);

    free(wasmBinary);

    if (state == CWebAssemblyLoadState::Failed)
    {
        CLogger::LogPrintf("Error while loading web assembly module on server [\"%s\"].\n", filePath.c_str());
    }

    return 0;
}

int CResourceManifest::Lua_WasmHasFileValidBinary(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) != LUA_TSTRING)
    {
        CLogger::ErrorPrintf("`wasm_has_file_valid_binary` function expects a `string` as first argument.\n");

        lua_pushboolean(luaVM, false);

        return 1;
    }

    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CResource* resource = pManifest->GetResource();

    const char* fileName = lua_tostring(luaVM, 1);

    SString path = "";

    bool exists = resource->GetFilePath(fileName, path);

    if (!exists)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    FILE* file = File::Fopen(path.c_str(), "rb");

    if (!file)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t bufferSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const size_t  WASM_BINARY_HEADER_SIZE = 5;
	unsigned char wasmHeader[WASM_BINARY_HEADER_SIZE] = {0, 97, 115, 109, 1};

    if (bufferSize < WASM_BINARY_HEADER_SIZE)
    {
        fclose(file);

        lua_pushboolean(luaVM, false);

        return 1;
    }

    char* buffer = (char*)malloc(WASM_BINARY_HEADER_SIZE);

    if (fread(buffer, WASM_BINARY_HEADER_SIZE, 1, file) != 1)
    {
        free(buffer);
        fclose(file);

        lua_pushboolean(luaVM, false);

        return 1;
    }

    fclose(file);

    bool result = std::string(buffer, WASM_BINARY_HEADER_SIZE) == std::string((char*)wasmHeader, WASM_BINARY_HEADER_SIZE);

    lua_pushboolean(luaVM, result);

    return 1;
}
