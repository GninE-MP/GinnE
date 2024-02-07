/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Main.cpp
 *  PURPOSE:     GninE loader
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#include "Main.h"
#include "CInstallManager.h"
#include "MainFunctions.h"
#include "Dialogs.h"
#include "Utils.h"
#include "SharedUtil.Win32Utf8FileHooks.hpp"
#if defined(GninE_DEBUG)
    #include "SharedUtil.Tests.hpp"
#endif
#include <version.h>

///////////////////////////////////////////////////////////////
//
// DoWinMain
//
// 'GninE San Andreas.exe' is launched as a subprocess under the following circumstances:
//      1. During install with /kdinstall command (as admin)
//      2. During uninstall with /kduninstall command (as admin)
//      3. By 'GninE San Andreas.exe' when temporary elevated privileges are required (as admin)
//      4. By 'GninE San Andreas.exe' during auto-update (in a temporary directory somewhere) (Which may then call it again as admin)
//
///////////////////////////////////////////////////////////////
GninEEXPORT int DoWinMain(HINSTANCE hLauncherInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AddUtf8FileHooks();

#if defined(GninE_DEBUG)
    SharedUtil_Tests();
#endif

    //
    // Init
    //

    // Let install manager figure out what GninESA path to use
    GetInstallManager()->SetGninESAPathSource(lpCmdLine);

    // Start logging.....now
    BeginEventLog();

    // Start localization if possible
    InitLocalization(false);

    // Handle commands from the installer
    HandleSpecialLaunchOptions();

    // Check GninE is launched only once
    HandleDuplicateLaunching();

    // Show logo
    ShowSplash(g_hInstance);

    // Other init stuff
    ClearPendingBrowseToSolution();

    // Find GTA path to use
    ValidateGTAPath();

    //
    // Update
    //

    // Continue any update procedure
    SString strCmdLine = GetInstallManager()->Continue();

    //
    // Launch
    //

    // Ensure localization is started
    InitLocalization(true);

    // Setup/test various counters and flags for monitoring problems
    PreLaunchWatchDogs();

    // Stuff
    HandleCustomStartMessage();
    #if !defined(GninE_DEBUG) && GninESA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    ForbodenProgramsMessage();
    #endif
    CycleEventLog();
    BsodDetectionPreLaunch();
    MaybeShowCopySettingsDialog();

    // Make sure GTA is not running
    HandleIfGTAIsAlreadyRunning();

    // Maybe warn user if no anti-virus running
    CheckAntiVirusStatus();

    // Ensure logo is showing
    ShowSplash(g_hInstance);

    // Check GninE files look good
    CheckDataFiles();
    CheckLibVersions();

    // Go for launch
    int iReturnCode = LaunchGame(strCmdLine);

    PostRunWatchDogs(iReturnCode);

    //
    // Quit
    //

    HandleOnQuitCommand();

    // Maybe show help if trouble was encountered
    ProcessPendingBrowseToSolution();

    AddReportLog(1044, SString("* End (0x%X)* pid:%d", iReturnCode, GetCurrentProcessId()));

    RemoveUtf8FileHooks();
    return iReturnCode;
}
