/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/Core.cpp
 *  PURPOSE:     Core library entry point
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <filesystem>
#define DECLARE_PROFILER_SECTION_Core
#include "profiler/SharedUtil.Profiler.h"
#define UTF8_FILE_HOOKS_PERSONALITY_Core
#include "SharedUtil.Win32Utf8FileHooks.hpp"

#define CORE_API extern "C" __declspec(dllexport)

namespace fs = std::filesystem;

CCore*         g_pCore = NULL;
CGraphics*     g_pGraphics = NULL;
CLocalization* g_pLocalization = NULL;

HINSTANCE g_hModule = NULL;

fs::path g_gtaDirectory;
fs::path g_GninEDirectory;

BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_hModule = dll;
        DisableThreadLibraryCalls(dll);
        AddUtf8FileHooks();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        RemoveUtf8FileHooks();

        if (IsGTAProcess())
        {
            // For now, ExitProcess if any destruction is attempted (or we'll crash)
            ExitProcess(0);

            if (g_pCore)
            {
                AddReportLog(7102, "Core - PROCESS_DETACH");

                delete g_pCore;
                g_pCore = nullptr;
            }
        }
    }

    return TRUE;
}

CORE_API void SetGninEDirectory(const wchar_t* path, size_t length)
{
    if (!g_GninEDirectory.empty() || !path || !path[0] || !length)
        return;

    g_GninEDirectory.assign(std::wstring_view{path, length});
}

CORE_API void SetGTADirectory(const wchar_t* path, size_t length)
{
    if (!g_gtaDirectory.empty() || !path || !path[0] || !length)
        return;

    g_gtaDirectory.assign(std::wstring_view{path, length});
}

CORE_API int InitializeCore()
{
    if (g_pCore)
        return 1;

    if (!IsGTAProcess())
        return 2;

    if (g_GninEDirectory.empty())
        return 3;

    if (g_gtaDirectory.empty())
        return 4;

    // Group our processes and windows under a single taskbar button
    SetCurrentProcessExplicitAppUserModelID(L"GninE");

    WriteDebugEvent(SString("ModuleFileName: %s", *GetLaunchPathFilename()));

    g_pCore = new CCore();
    return 0;
}
