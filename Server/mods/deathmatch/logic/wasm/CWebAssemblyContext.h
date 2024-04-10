/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasm/CWebAssemblyContext.h
 *  PURPOSE:     Resource handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef C_WEB_ASSEMBLY_CONTEXT_HEADER
#define C_WEB_ASSEMBLY_CONTEXT_HEADER

#include <vector>

#include "WebAssemblyImports.h"

typedef wasm_engine_t*       CWebAssemblyEngineContext;
typedef wasm_store_t*        CWebAssemblyStoreContext;
typedef wasm_func_callback_t CWebAssemblyCFunction;
typedef wasm_trap_t          CWebAssemblyTrap;
typedef wasm_module_t*       CWebAssemblyModuleContext;
typedef wasm_instance_t*     CWebAssemblyInstanceContext;

typedef std::vector<CWebAssemblyCFunction> CWebAssemblyCFunctionList;
typedef std::vector<wasm_extern_t*>        CWebAssemblyImports;

enum class CWebAssemblyLoadState
{
    Succeed,
    Failed
};

class CWebAssemblyEngine
{
public:
    CWebAssemblyEngine();
    CWebAssemblyEngine(const CWebAssemblyEngineContext& context);
    ~CWebAssemblyEngine();

    void Build();
    void Destroy();

    void                      SetContext(const CWebAssemblyEngineContext& context);
    CWebAssemblyEngineContext GetContext();

    operator bool();

private:
    CWebAssemblyEngineContext m_pContext;
};

class CWebAssemblyStore
{
public:
    CWebAssemblyStore();
    CWebAssemblyStore(const CWebAssemblyStoreContext& context);
    CWebAssemblyStore(CWebAssemblyEngine* engine);
    ~CWebAssemblyStore();

    void Build(CWebAssemblyEngine* engine);
    void Build();
    void Destroy();

    void                     SetContext(const CWebAssemblyStoreContext& context);
    CWebAssemblyStoreContext GetContext();

    void                SetEngine(CWebAssemblyEngine* engine);
    CWebAssemblyEngine* GetEngine();

    operator bool();

private:
    CWebAssemblyStoreContext m_pContext;

    CWebAssemblyEngine* m_waEngine;
};

class CWebAssemblyContext
{
public:
    CWebAssemblyContext();
    ~CWebAssemblyContext();

    CWebAssemblyLoadState LoadBinary(const char* binary, const size_t& binarySize);
    CWebAssemblyLoadState Load();
    void                  Unload();

    void                 Import(const CWebAssemblyCFunction& function);
    void                 ClearImports();
    CWebAssemblyImports& GetImports();

    void        SetModuleBinary(const char* binary);
    const char* GetModuleBinary();
    void        SetModuleBinarySize(const size_t& binarySize);
    size_t      GetModuleBinarySize();
    void        FreeModuleBinary();

    CWebAssemblyStore* GetStore();

    void ReleaseBinary();

    bool IsLoaded();

    static void                InitializeWebAssemblyEngine();
    static void                DeleteWebAssemblyEngine();
    static CWebAssemblyEngine* GetWebAssemblyEngine();

    static bool IsWebAssemblyBinary(const char* binary);

private:
    char*  m_pModuleBinary;
    size_t m_uiModuleBinarySize;

    bool m_bLoaded;

    CWebAssemblyStore* m_pStore;

    CWebAssemblyImports m_lsImports;
};

#endif
