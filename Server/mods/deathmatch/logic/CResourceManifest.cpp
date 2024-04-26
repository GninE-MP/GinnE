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
#include "CResourceWasmScriptItem.h"
#include "CResourceWasmClientScriptItem.h"

#include "wasm/WebAssemblyImports.h"

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

#define WASM_COMPILER_SETTING_FLAG "-s"
#define WASM_COMPILER_OUT_FLAG     "-o"
#define WASM_COMPILER_INCLUDE_FLAG "-I"
#define WASM_COMPILER_MACRO_FLAG   "-D"

#define WASM_COMPILER_DEFAULT_INCLUDE_DIR "mods/deathmatch/wasm/include"
#define WASM_COMPILER_DEFAULT_OPTIMIZE_LEVEL "-Os"
#define WASM_COMPILER_DEFAULT_DEVELOPMENT_LEVEL "-O0"

#define WINDOWS_PATH_ENVIRONMENT_VARIABLE_NAME "PATH"

std::unordered_map<SString, SString> DEFAULT_WASM_COMPILER_SETTINGS = {
    { "STANDALONE_WASM", "1" },
    { "TOTAL_STACK", "65536" },
    { "TOTAL_MEMORY", "1048576" },
    { "ERROR_ON_UNDEFINED_SYMBOLS", "0" }
};

std::vector<SString> DEFAULT_WASM_EXPORT_FUNCTIONS = {
    WASM_MAIN_FUNCTION_NAME,
    WASM_MALLOC_FUNCTION_NAME,
    WASM_FREE_FUNCTION_NAME
};

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

void CResourceManifest::AddWasmClientScript(CResourceManifest* manifest, const char* fileName)
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
            CResourceWasmClientScriptItem* scriptItem = new CResourceWasmClientScriptItem(pResource, fileNameFixed.c_str(), fileFullPath.c_str(), &attributes);

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

void CResourceManifest::AddWasmServerScript(CResourceManifest* manifest, const char* fileName)
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
            CResourceWasmScriptItem* scriptItem = new CResourceWasmScriptItem(pResource, fileNameFixed.c_str(), fileFullPath.c_str(), &attributes);

            manifest->GetResourceFiles().push_back(scriptItem);
            pResource->GetFiles().push_back(scriptItem);

            pResource->GenerateChecksumForFile(scriptItem).get();
        }
    }

    delete node;
}


SString CResourceManifest::GetApplicationDirectory(char concatenator)
{
    SString result = "";

    char name[256];

    int byteSize = GetModuleFileName(NULL, name, sizeof(name));

    if (byteSize < 1)
    {
        return result;
    }

    result = std::string(name, byteSize);

    std::vector<SString> pathes;
    result.Split("\\", pathes);

    pathes.pop_back();

    size_t length = pathes.size();

    SString path = "";

    for (size_t i = 0; i < length; i++)
    {
        path += pathes[i];
        path += concatenator;
    }

    if (!path.empty())
    {
        path.pop_back();
    }

    return path;
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
    if (lua_type(luaVM, 1) != LUA_TTABLE)
    {
        CLogger::ErrorPrintf("`wasm_compile` function expected a 'table' value as first argument.\n");
        return 0;
    }

    lua_getglobal(luaVM, RESOURCE_MAIN_MANIFEST_OBJECT_GLOBAL_NAME);

    CResourceManifest* pManifest = (CResourceManifest*)lua_touserdata(luaVM, -1);

    lua_pop(luaVM, 1);

    if (!pManifest)
    {
        return 0;
    }

    CResource* pResource = pManifest->GetResource();

    std::unordered_map<SString, SString> settings;
    std::vector<SString>                 exports;
    std::vector<SString>                 sources;

    SString out = "";
    SString optimizingLevel = WASM_COMPILER_DEFAULT_OPTIMIZE_LEVEL;

    std::vector<SString> includeDirectories;

    SString defaultDir = GetApplicationDirectory();
    defaultDir += "/";
    defaultDir += WASM_COMPILER_DEFAULT_INCLUDE_DIR;

    includeDirectories.push_back(defaultDir);

    SString command = WASM_COMPILER_COMMAND;
    command += " --no-entry";

    lua_pushstring(luaVM, "development");
    lua_gettable(luaVM, -2);

    if (lua_toboolean(luaVM, -1))
    {
        optimizingLevel = WASM_COMPILER_DEFAULT_DEVELOPMENT_LEVEL;
    }

    lua_pop(luaVM, 1);

    for (const std::pair<SString, SString>& setting : DEFAULT_WASM_COMPILER_SETTINGS)
    {
        settings[setting.first] = setting.second;
    }

    for (const SString& functionName : DEFAULT_WASM_EXPORT_FUNCTIONS)
    {
        exports.push_back(functionName);
    }

    lua_pushstring(luaVM, "settings");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        lua_pushnil(luaVM);

        while (lua_next(luaVM, -2))
        {
            SString key = lua_tostring(luaVM, -2);
            SString value = lua_tostring(luaVM, -1);

            settings[key] = value;

            lua_pop(luaVM, 1);
        }
    }

    lua_pop(luaVM, 1);

    lua_pushstring(luaVM, "exports");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t length = lua_objlen(luaVM, -1);

        for (size_t i = 1; i <= length; i++)
        {
            lua_pushinteger(luaVM, i);
            lua_gettable(luaVM, -2);

            if (lua_type(luaVM, -1) == LUA_TSTRING)
            {
                SString function = lua_tostring(luaVM, -1);

                exports.push_back(function);
            }

            lua_pop(luaVM, 1);
        }
    }

    lua_pop(luaVM, 1);

    lua_pushstring(luaVM, "include");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t length = lua_objlen(luaVM, -1);

        for (size_t i = 1; i <= length; i++)
        {
            lua_pushinteger(luaVM, i);
            lua_gettable(luaVM, -2);

            if (lua_type(luaVM, -1) == LUA_TSTRING)
            {
                SString directory = lua_tostring(luaVM, -1);

                includeDirectories.push_back(directory);
            }

            lua_pop(luaVM, 1);
        }
    }

    lua_pop(luaVM, 1);

    lua_pushstring(luaVM, "src");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) != LUA_TTABLE)
    {
        goto SourceFilesError;
    }

    size_t srcLength = lua_objlen(luaVM, -1);

    if (srcLength < 1)
    {
        goto SourceFilesError;
    }

    for (size_t i = 1; i <= srcLength; i++)
    {
        lua_pushinteger(luaVM, i);
        lua_gettable(luaVM, -2);

        if (lua_type(luaVM, -1) == LUA_TSTRING)
        {
            SString src = lua_tostring(luaVM, -1);

            SString path = "";

            if (!pResource->GetFilePath(src.c_str(), path))
            {
                lua_pop(luaVM, 2);

                CLogger::ErrorPrintf("`wasm_compile` source file doesn't exist['%s'].\n", src.c_str());

                return 0;
            }

            size_t length = path.length();

            for (size_t j = 0; j < length; j++)
            {
                if (path[j] == '\\')
                {
                    path[j] = '/';
                }
            }

            sources.push_back(path);
        }

        lua_pop(luaVM, 1);
    }

    lua_pop(luaVM, 1);

    lua_pushstring(luaVM, "out");
    lua_gettable(luaVM, -2);

    if (!(lua_type(luaVM, -1) == LUA_TSTRING && lua_strlen(luaVM, -1) > 0))
    {
        lua_pop(luaVM, 1);

        CLogger::ErrorPrintf("`wasm_compile` '.out' property must be a string value.\n");

        return 0;
    }

    SString* outPath = new SString();

    pResource->GetFilePath(lua_tostring(luaVM, -1), *outPath);

    {
        size_t length = outPath->length();

        for (size_t i = 0; i < length; i++)
        {
            if ((*outPath)[i] == '\\')
            {
                (*outPath)[i] = '/';
            }
        }
    }

    out = *outPath;

    delete outPath;

    lua_pop(luaVM, 1);

    lua_pushstring(luaVM, "optimize_level");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) == LUA_TSTRING)
    {
        optimizingLevel = lua_tostring(luaVM, -1);
    }

    lua_pop(luaVM, 1);

    for (const std::pair<SString, SString>& setting : settings)
    {
        command += " ";
        command += WASM_COMPILER_SETTING_FLAG;
        command += " \"";
        command += setting.first;
        command += "=";
        command += setting.second;
        command += "\"";
    }

    if (!exports.empty())
    {
        command += " ";
        command += WASM_COMPILER_SETTING_FLAG;
        command += " \"EXPORTED_FUNCTIONS=[";

        for (const SString& function : exports)
        {
            command += "'_";
            command += function;
            command += "',";
        }

        command.pop_back();

        command += "]\"";
    }

    command += " ";
    command += optimizingLevel;

    command += " ";
    command += WASM_COMPILER_OUT_FLAG;
    command += " \"";
    command += out;
    command += "\"";

    for (const SString& src : sources)
    {
        command += " \"";
        command += src;
        command += "\"";
    }

    for (const SString& directory : includeDirectories)
    {
        command += " ";
        command += WASM_COMPILER_INCLUDE_FLAG;
        command += "\"";
        command += directory;
        command += "\"";
    }

    lua_pushstring(luaVM, "macro");
    lua_gettable(luaVM, -2);

    if (lua_type(luaVM, -1) == LUA_TTABLE)
    {
        size_t length = lua_objlen(luaVM, -1);

        if (length > 0)
        {
            lua_pop(luaVM, 1);

            CLogger::ErrorPrintf("`wasm_compile` '.macro' is a map table. It can't have number indexes!\n");

            return 0;
        }

        lua_pushnil(luaVM);

        while (lua_next(luaVM, -2))
        {
            if (lua_type(luaVM, -2) != LUA_TSTRING)
            {
                continue;
            }
            
            SString name = lua_tostring(luaVM, -2);
            SString value = lua_tostring(luaVM, -1);

            command += " ";
            command += WASM_COMPILER_MACRO_FLAG;
            command += "\"";
            command += name;
            command += "=";
            command += value;
            command += "\"";

            lua_pop(luaVM, 1);
        }
    }

    lua_pop(luaVM, 1);

    FILE* pipe = _popen(command.c_str(), "r");

    if (!pipe)
    {
        return 0;
    }

    _pclose(pipe);

    return 0;

SourceFilesError:
    lua_pop(luaVM, 1);

    CLogger::ErrorPrintf("`wasm_compile` couldn't read source files! [`.src` must be a table value]\n");

    return 0;
}

int CResourceManifest::Lua_WasmClient(lua_State* luaVM)
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

        AddWasmClientScript(pManifest, fileName);
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

                AddWasmClientScript(pManifest, fileName);
            }

            lua_pop(luaVM, 1);
        }
    }

    return 0;
}

/*WebAssemblyApi(GnineGetData, env, args, results)
{
    CResource* resource = GetWebAssemblyResource(env);
    CWebAssemblyScript* script = GetWebAssemblyScript(env);

    int arg0 = args->data[0].of.i32;
    int arg1 = args->data[1].of.i32;

    CLogger::LogPrintf("hi this is get data on '%s' and arg is in new resource : %d and script is '%s'!\n", resource->GetName().c_str(), arg0, script->GetScriptFile().c_str());

    CWebAssemblyVariables funcArgs, funcResults;

    funcArgs.Push(3050);

    script->CallInternalFunction(arg1, &funcArgs, &funcResults);

    CWebAssemblyMemory* memory = script->GetMemory();

    int func = funcResults.GetFirst().GetInt32();

    for (int i = 0; i < 5; i++)
    {
        funcArgs.Clear();
        funcResults.Clear();

        SString data = "mobin = ";
        data += std::to_string(i);

        CWebAssemblyMemoryPointerAddress str = memory->StringToUTF8(data);

        funcArgs.Push((int32_t)str);
        funcArgs.Push(13 + i);

        script->CallInternalFunction(func, &funcArgs, &funcResults);

        memory->Free(str);

        CLogger::LogPrintf("function call result is : %f\n", funcResults.GetFirst().GetFloat32());
    }
    
    return NULL;
}

WebAssemblyApi(GninePrintData, env, args, results)
{
    CWebAssemblyMemoryPointerAddress formaterPointer = args->data[0].of.i32;

    CWebAssemblyScript* script = GetWebAssemblyScript(env);
    CWebAssemblyMemory* memory = script->GetMemory();

    SString format = "INFO: ";
    format += memory->UTF8ToString(formaterPointer);
    format += "\n";

    CLogger::LogPrintf(format.c_str());
    
    return NULL;
}

WebAssemblyApi(GnineCallFunction, env, args, results)
{
    CWebAssemblyScript* script = GetWebAssemblyScript(env);

    int refAddress = args->data[0].of.i32;

    CWebAssemblyMemoryPointerAddress arguments = args->data[1].of.i32;

    wasm_ref_t* ref = (wasm_ref_t*)((uintptr_t)refAddress);

    wasm_func_t* func = wasm_ref_as_func(ref);

    CWebAssemblyFunctionType fType;
    fType.ReadFunctionTypeContext(wasm_func_type(func));
    
    CWebAssemblyFunction f;
    f.SetApiEnviornment(script);
    f.SetFunctionContext(func);
    f.SetFunctionType(fType);
    f.SetStore(script->GetStoreContext()->GetStore());

    CWebAssemblyVariables fArgs;

    CWebAssemblyMemoryPointerAddress str = script->GetMemory()->StringToUTF8("salam in call az ref omade!");

    fArgs.Push((int32_t)str);

    f.Call(&fArgs, NULL);

    script->GetMemory()->Free(str);

    struct Argument
    {
        CWebAssemblyMemoryPointerAddress value;
        unsigned char                    type;
    };

    struct Arguments
    {
        CWebAssemblyMemoryPointerAddress args;
        uint64_t                         length;
    };

    Arguments* wArgs = (Arguments*)script->GetMemory()->GetMemoryPhysicalPointer(arguments);

    CWebAssemblyMemoryPointerAddress* argsPtrList = (CWebAssemblyMemoryPointerAddress*)script->GetMemory()->GetMemoryPhysicalPointer(wArgs->args);

    for (uint64_t i = 0; i < wArgs->length; i++)
    {
        Argument* arg = (Argument*)script->GetMemory()->GetMemoryPhysicalPointer(argsPtrList[i]);

        void* value = script->GetMemory()->GetMemoryPhysicalPointer(arg->value);

        switch ((int)arg->type)
        {
            case 0: // null
                CLogger::LogPrintf("%d) NULL\n", i);
                break;
            case 1: // i32
                CLogger::LogPrintf("%d) i32 = %d\n", i, *(int32_t*)value);
                break;
            case 2: // i64
                CLogger::LogPrintf("%d) i64 = %I64d\n", i, *(int64_t*)value);
                break;
            case 3: // f32
                CLogger::LogPrintf("%d) f32 = %f\n", i, *(float*)value);
                break;
            case 4: // f64
                CLogger::LogPrintf("%d) f64 = %g\n", i, *(double*)value);
                break;
            case 5: // str
                CLogger::LogPrintf("%d) string = '%s'\n", i, script->GetMemory()->UTF8ToString(arg->value).c_str());
                break;
            case 6: // internal function
                CLogger::LogPrintf("%d) internal function = %d\n", i, *(CWebAssemblyMemoryPointerAddress*)value);
                break;
            case 7: // pointer address
                CLogger::LogPrintf("%d) pointer stack = %d, real = %d\n", i, *(CWebAssemblyMemoryPointerAddress*)value, script->GetMemory()->GetMemoryPhysicalPointer(*(CWebAssemblyMemoryPointerAddress*)value));
                break;
            case 8: // arglist
                CLogger::LogPrintf("%d) we have an arg list!\n", i);
                break;
            default:
                break;
        }
    }

    return NULL;
}
*/
int CResourceManifest::Lua_WasmServer(lua_State* luaVM)
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
        
        AddWasmServerScript(pManifest, fileName);
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

                AddWasmServerScript(pManifest, fileName);
            }

            lua_pop(luaVM, 1);
        }
    }

    return 0;
    /*if (lua_type(luaVM, -1) != LUA_TSTRING)
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

    // create `CResourceWasmScriptItem` class!*/ 

    /*FILE* wasmFile = File::Fopen(path.c_str(), "rb");

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

    CWebAssemblyContext* wasmModule = new CWebAssemblyContext(pResource);

    CWebAssemblyScript* script = wasmModule->CreateScript();

    CWebAssemblyVariables args, results;

    args.PushInt32();
    args.PushInt32();

    CWebAssemblyFunctionType functionType(args, results);

    script->RegisterApiFunction("gnine_get_data", functionType, GnineGetData);

    CWebAssemblyVariables pargs, presults;

    pargs.PushInt32();

    //for (int i = 0; i < 16; i++) pargs.PushFloat64();

    CWebAssemblyFunctionType printType(pargs, presults);

    script->RegisterApiFunction("gnine_print_data", printType, GninePrintData);

    CWebAssemblyVariables cArgs, cResults;

    //cArgs.PushAnyRef();
    cArgs.PushInt32();
    cArgs.PushPointer();
    
    CWebAssemblyFunctionType CallbackCallerType(cArgs, cResults);

    script->RegisterApiFunction("gnine_call_callback", CallbackCallerType, GnineCallFunction);
    
    CWebAssemblyLoadState state = wasmModule->LoadScriptBinary(script, wasmBinary, bufferSize, filePath);

    CWebAssemblyMemory*   memory = script->GetMemory();
    CWebAssemblyFunction* getResourceName = script->GetExportedFunction("get_resource_name");

    CWebAssemblyVariables fArgs, fResults;

    CWebAssemblyFunction* printFunc = script->GetApiFunction("gnine_print_data");
    
    wasm_func_t* funcC = printFunc->GetFunctionContext();
    wasm_ref_t*  ref = wasm_func_as_ref(funcC);*/ 

    /*wasm_val_t v;
    v.kind = WASM_ANYREF;
    v.of.ref = ref;

    CWebAssemblyVariable var;
    var.Set(v);*/ 

    /*fArgs.Push(ref);

    getResourceName->Call(&fArgs, &fResults);

    SString name = memory->UTF8ToString(fResults.GetFirst().GetInt32());

    CLogger::LogPrintf("res name is d: %s\n", name.c_str());*/ 

    /*char* dataPointer = (char*)mem->GetBase() + 67112;

    //wasm_runtime_module_malloc(script->GetModule(), 300, NULL);
    
    std::string data(dataPointer, 10);

    CLogger::LogPrintf("memory data is : %s\n", data.c_str());

    void* memoryData = NULL;

    CWebAssemblyMemoryPointerAddress modulePtr = mem->Malloc(5, &memoryData);

    CLogger::LogPrintf("module allocating pointer is : %d and real memory data is : %d\n", modulePtr, memoryData);

    mem->Free(modulePtr);*/

    /*free(wasmBinary);

    delete wasmModule;

    if (state == CWebAssemblyLoadState::Failed)
    {
        CLogger::LogPrintf("Error while loading web assembly module on server [\"%s\"].\n", filePath.c_str());
    }
    
    return 0;*/
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
