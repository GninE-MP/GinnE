/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyFileDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#ifndef GninE_CLIENT
    #include "CChecksum.h"
#endif

#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CDummy.h"
#include "CStaticFunctionDefinitions.h"
#include "Utils.h"
#include "CScriptDebugging.h"

#include "CScriptFile.h"
#include "CScriptArgReader.h"
#include "CResourceManager.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyFileDefs.h"

#define DEFAULT_MAX_FILESIZE 52428800

#define FILE_INVALID_POSITION ((int64_t)-1)
#define FILE_INVALID_SIZE     ((int64_t)-1)

void CWebAssemblyFileDefs::RegisterFunctionTypes()
{
    SetFunctionType("file_open", "esb");
    SetFunctionType("file_create", "es");
    SetFunctionType("file_exists", "bs");
    SetFunctionType("file_copy", "bssb");
    SetFunctionType("file_rename", "bss");
    SetFunctionType("file_delete", "bs");

    SetFunctionType("file_close", "be");
    SetFunctionType("file_flush", "be");
    SetFunctionType("file_read", "xexsx");
    SetFunctionType("file_write", "xesx");
    SetFunctionType("file_get_contents", "xesxb");

    SetFunctionType("file_get_pos", "le");
    SetFunctionType("file_get_size", "le");
    SetFunctionType("file_get_path", "xesx");

    SetFunctionType("file_is_eof", "be");

    SetFunctionType("file_set_pos", "lel");
}

void CWebAssemblyFileDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "file_open", FileOpen },
        { "file_create", FileCreate },
        { "file_exists", FileExists },
        { "file_copy", FileCopy },
        { "file_rename", FileRename },
        { "file_delete", FileDelete },

        { "file_close", FileClose },
        { "file_flush", FileFlush },
        { "file_read", FileRead },
        { "file_write", FileWrite },
        { "file_get_contents", FileGetContents },

        { "file_get_pos", FileGetPos },
        { "file_get_size", FileGetSize },
        { "file_get_path", FileGetPath },

        { "file_is_eof", FileIsEOF },

        { "file_set_pos", FileSetPos }
    };

    WASM_REGISTER_API(script, functions);
}

static auto getResourceFilePath(CResource* thisResource, CResource* fileResource, const SString& relativePath) -> SString
{
    if (thisResource == fileResource)
        return relativePath;

#ifdef GninE_CLIENT
    return SString(":%s/%s", fileResource->GetName(), relativePath.c_str());
#else
    return SString(":%s/%s", fileResource->GetName().c_str(), relativePath.c_str());
#endif
};

WebAssemblyApi(CWebAssemblyFileDefs::FileOpen, env, args, results)
{
    SString filePath;
    bool    readOnly;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);
    argStream.ReadBoolean(readOnly, false);

    CLuaMain*  luaMain = GetWebAssemblyLuaMain(env);
    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    if (!luaMain || !luaVM)
    {
        return argStream.ReturnNull();
    }
    
    SString absPath;
    SString metaPath;

    CResource* resource = luaMain->GetResource();
    CResource* pResource = resource;

    if (!CResourceManager::ParseResourcePathInput(filePath, pResource, &absPath, &metaPath))
    {
        return argStream.ReturnNull();
    }

    CScriptArgReader luaArgReader(luaVM);

    CheckCanModifyOtherResource(luaArgReader, resource, pResource);
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pResource, absPath, &readOnly);

#ifndef GninE_CLIENT
    CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE);
#else
    eAccessType  accessType = filePath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
    CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE, accessType);
#endif

    if (!file->Load(pResource, readOnly ? CScriptFile::MODE_READ : CScriptFile::MODE_READWRITE))
    {
        delete file;
        return argStream.ReturnNull();
    }
    
#ifdef GninE_CLIENT
    file->SetParent(pResource->GetResourceDynamicEntity());
    file->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
    file->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif

    CResource* parentResource = luaMain->GetResource();

    if (parentResource)
    {
        CElementGroup* group = parentResource->GetElementGroup();

        if (group)
        {
            group->Add(file);
        }
    }

    return argStream.Return((CElement*)file);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileCreate, env, args, results)
{
    SString filePath;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);
    
    CLuaMain*  luaMain = GetWebAssemblyLuaMain(env);
    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    if (!luaMain || !luaVM)
    {
        return argStream.ReturnNull();
    }

    CScriptArgReader luaArgReader(luaVM);

#ifdef GninE_CLIENT
    if (!g_pNet->ValidateBinaryFileName(filePath))
    {
        return argStream.ReturnNull();
    }
#endif

    SString absPath;
    SString metaPath;

    CResource* resource = luaMain->GetResource();
    CResource* pResource = resource;

    if (!CResourceManager::ParseResourcePathInput(filePath, pResource, &absPath, &metaPath))
    {
        return argStream.ReturnNull();
    }

    CheckCanModifyOtherResource(luaArgReader, resource, pResource);
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pResource, absPath);

#ifdef GninE_CLIENT
    g_pClientGame->GetResourceManager()->OnFileModifedByScript(filePath, "fileCreate");
#endif

    MakeSureDirExists(absPath);

#ifdef GninE_CLIENT
    eAccessType  accessType = filePath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
    CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE, accessType);
#else
    CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE);
#endif

    if (!file->Load(pResource, CScriptFile::MODE_CREATE))
    {
        delete file;
        return argStream.ReturnNull();
    }
    
#ifdef GninE_CLIENT
    file->SetParent(pResource->GetResourceDynamicEntity());
    file->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
    file->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(file);
    }

    return argStream.Return((CElement*)file);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileExists, env, args, results)
{
    SString filePath;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);
    
    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.Return(false);
    }

    SString absPath;

    CResource* resource = luaMain->GetResource();

    if (!CResourceManager::ParseResourcePathInput(filePath, resource, &absPath))
    {
        return argStream.Return(false);
    }

    return argStream.Return(SharedUtil::FileExists(absPath));
}

WebAssemblyApi(CWebAssemblyFileDefs::FileCopy, env, args, results)
{
    SString filePath;
    SString newFilePath;
    bool    overwrite;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);
    argStream.ReadString(newFilePath);
    argStream.ReadBoolean(overwrite, false);

    CLuaMain*  luaMain = GetWebAssemblyLuaMain(env);
    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    if (!luaMain || !luaVM)
    {
        argStream.Return(false);
    }

#ifdef GninE_CLIENT
    if (!g_pNet->ValidateBinaryFileName(newFilePath))
    {
        return argStream.Return(false);
    }
#endif

    SString srcAbsPath;
    SString destAbsPath;

    CResource* resource = luaMain->GetResource();
    CResource* pSrcResource = resource;
    CResource* pDestResource = resource;

    if (
        !CResourceManager::ParseResourcePathInput(filePath, pSrcResource, &srcAbsPath) ||
        !CResourceManager::ParseResourcePathInput(newFilePath, pDestResource, &destAbsPath)
    )
    {
        return argStream.Return(false);
    }

    CScriptArgReader luaArgReader(luaVM);
    
    CheckCanModifyOtherResources(luaArgReader, resource, { pSrcResource, pDestResource });
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pSrcResource, srcAbsPath);
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pDestResource, destAbsPath);

    if (!SharedUtil::FileExists(srcAbsPath))
    {
        return argStream.Return(false);
    }
    
    if (!(overwrite || !SharedUtil::FileExists(destAbsPath)))
    {
        return argStream.Return(false);
    }
    
#ifdef GninE_CLIENT
    g_pClientGame->GetResourceManager()->OnFileModifedByScript(destAbsPath, "fileCopy");
#endif

    MakeSureDirExists(destAbsPath);

    if (!SharedUtil::FileCopy(srcAbsPath, destAbsPath))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileRename, env, args, results)
{
    SString filePath;
    SString newFilePath;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);
    argStream.ReadString(newFilePath);

    CLuaMain*  luaMain = GetWebAssemblyLuaMain(env);
    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    if (!luaMain || !luaVM)
    {
        return argStream.Return(false);
    }

#ifdef GninE_CLIENT
    if (!g_pNet->ValidateBinaryFileName(newFilePath))
    {
        return argStream.Return(false);
    }
#endif

    SString srcAbsPath;
    SString destAbsPath;

    CResource* resource = luaMain->GetResource();
    CResource* pSrcResource = resource;
    CResource* pDestResource = resource;

    if (
        !CResourceManager::ParseResourcePathInput(filePath, pSrcResource, &srcAbsPath) ||
        !CResourceManager::ParseResourcePathInput(newFilePath, pDestResource, &destAbsPath)
    )
    {
        return argStream.Return(false);
    }

    CScriptArgReader luaArgReader(luaVM);
    
    CheckCanModifyOtherResources(luaArgReader, resource, { pSrcResource, pDestResource });
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pSrcResource, srcAbsPath);
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pDestResource, destAbsPath);

    if (!SharedUtil::FileExists(srcAbsPath))
    {
        return argStream.Return(false);
    }
    
    if (SharedUtil::FileExists(destAbsPath))
    {
        return argStream.Return(false);
    }
    
#ifdef GninE_CLIENT
    g_pClientGame->GetResourceManager()->OnFileModifedByScript(srcAbsPath, "fileRename");
#endif

    MakeSureDirExists(destAbsPath);

    return argStream.Return(SharedUtil::FileRename(srcAbsPath, destAbsPath));
}

WebAssemblyApi(CWebAssemblyFileDefs::FileDelete, env, args, results)
{
    SString filePath;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(filePath);

    CLuaMain*  luaMain = GetWebAssemblyLuaMain(env);
    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    if (!luaMain || !luaVM)
    {
        return argStream.Return(false);
    }
    
    SString absPath;

    CResource* resource = luaMain->GetResource();
    CResource* pResource = resource;

    if (!CResourceManager::ParseResourcePathInput(filePath, pResource, &absPath))
    {
        return argStream.Return(false);
    }

    CScriptArgReader luaArgReader(luaVM);
    
    CheckCanModifyOtherResource(luaArgReader, resource, pResource);
    CheckCanAccessOtherResourceFile(luaArgReader, resource, pResource, absPath);
    
#ifdef GninE_CLIENT
    g_pClientGame->GetResourceManager()->OnFileModifedByScript(absPath, "fileDelete");
#endif

    return argStream.Return(SharedUtil::FileDelete(absPath));
}

WebAssemblyApi(CWebAssemblyFileDefs::FileClose, env, args, results)
{
    CScriptFile* file;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);

    if (!file)
    {
        return argStream.Return(false);
    }

    file->Unload();

    g_pGame->GetElementDeleter()->Delete(file);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileFlush, env, args, results)
{
    CScriptFile* file;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);

    if (!file)
    {
        return argStream.Return(false);
    }

    file->Flush();

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileRead, env, args, results)
{
    CScriptFile*                     file;
    uint32_t                         byteCount;
    CWebAssemblyMemoryPointerAddress outContent;
    uint32_t                         maxContentSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);
    argStream.ReadUInt32(byteCount);
    argStream.ReadPointerAddress(outContent);
    argStream.ReadUInt32(maxContentSize, 0xffffffff);

    if (!file || outContent == WEB_ASSEMBLY_NULL_PTR || byteCount == 0)
    {
        return argStream.Return(0);
    }

    SString  buffer;
    uint32_t size = std::min(byteCount, maxContentSize);

    long bytesRead = file->Read((unsigned long)size, buffer);

    if (bytesRead < 0)
    {
        return argStream.Return(0);
    }

    argStream.WritePointer(outContent, buffer.data(), (intptr_t)bytesRead);

    return argStream.Return((uint32_t)bytesRead);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileWrite, env, args, results)
{
    CScriptFile* file;
    void*        content;
    uint32_t     contentSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);
    argStream.ReadPointer(content);
    argStream.ReadUInt32(contentSize);

    if (!file || !content || contentSize == 0)
    {
        return argStream.Return(0);
    }

    long bytesWritten = file->Write((unsigned long)contentSize, (const char*)content);
    
    if (bytesWritten < 0)
    {
        return argStream.Return(0);
    }

    return argStream.Return((uint32_t)bytesWritten);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileGetContents, env, args, results)
{
    CScriptFile*                     file;
    CWebAssemblyMemoryPointerAddress outContent;
    uint32_t                         maxContentSize;
    bool                             verifyContent;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);
    argStream.ReadPointerAddress(outContent);
    argStream.ReadUInt32(maxContentSize, 0xffffffff);
    argStream.ReadBoolean(verifyContent, true);

    std::string buffer;
    long        bytesRead;
    uint32_t    size;

    CResourceFile* resourceFile;

    CChecksum current;
    CChecksum expected;

    if (!file || outContent == WEB_ASSEMBLY_NULL_PTR || maxContentSize == 0)
    {
        goto Failed;
    }

    bytesRead = file->GetContents(buffer);

    if (bytesRead < 0)
    {
        goto Failed;
    }

    if (!verifyContent)
    {
        goto Success;
    }

    resourceFile = file->GetResourceFile();

    if (!resourceFile)
    {
        goto Failed;
    }

    current = CChecksum::GenerateChecksumFromBuffer(buffer.data(), static_cast<unsigned long>(buffer.size()));

#ifdef GninE_CLIENT
    expected = resourceFile->GetServerChecksum();
#else
    expected = resourceFile->GetLastChecksum();
#endif

    if (current == expected)
    {
        goto Success;
    }

Failed:
    return argStream.Return(0);

Success:
    size = std::min((uint32_t)bytesRead, maxContentSize);

    argStream.WritePointer(outContent, buffer.data(), (intptr_t)bytesRead);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileGetPos, env, args, results)
{
    CScriptFile* file;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);

    if (!file)
    {
        return argStream.Return(FILE_INVALID_POSITION);
    }

    int64_t position = file->GetPointer();

    if (position < 0)
    {
        return argStream.Return(FILE_INVALID_POSITION);
    }

    return argStream.Return(position);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileGetSize, env, args, results)
{
    CScriptFile* file;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);

    if (!file)
    {
        return argStream.Return(FILE_INVALID_SIZE);
    }

    int64_t size = file->GetSize();

    if (size < 0)
    {
        return argStream.Return(FILE_INVALID_SIZE);
    }

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileGetPath, env, args, results)
{
    CScriptFile*                     file;
    CWebAssemblyMemoryPointerAddress outPath;
    uint32_t                         maxPathSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);
    argStream.ReadPointerAddress(outPath);
    argStream.ReadUInt32(maxPathSize, 0xffffffff);

    if (!file || outPath == WEB_ASSEMBLY_NULL_PTR || maxPathSize == 0)
    {
        return argStream.Return(0);
    }

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.Return(0);
    }

    CResource* resource = luaMain->GetResource();
    CResource* fileResource = file->GetResource();

    SString filePath = getResourceFilePath(resource, fileResource, file->GetFilePath());

    uint32_t size = std::min(maxPathSize, (uint32_t)filePath.size());

    argStream.WritePointer(outPath, filePath.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyFileDefs::FileIsEOF, env, args, results)
{
    CScriptFile* file;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);

    if (!file)
    {
        return argStream.Return(false);
    }

    return argStream.Return(file->IsEOF());
}

WebAssemblyApi(CWebAssemblyFileDefs::FileSetPos, env, args, results)
{
    CScriptFile* file;
    int64_t      position;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(file);
    argStream.ReadInt64(position);

    if (!file)
    {
        return argStream.Return(FILE_INVALID_POSITION);
    }

    int64_t changedPosition = file->SetPointer(position);

    if (changedPosition < 0)
    {
        return argStream.Return(FILE_INVALID_POSITION);
    }

    return argStream.Return(changedPosition);
}
