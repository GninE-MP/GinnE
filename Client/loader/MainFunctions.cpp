/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/MainFunctions.cpp
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#include "MainFunctions.h"
#include "Main.h"
#include "Utils.h"
#include "Dialogs.h"
#include "D3DStuff.h"
#include <version.h>
#include <array>
#include <locale.h>

DECLARE_ENUM(WSC_SECURITY_PROVIDER_HEALTH)
IMPLEMENT_ENUM_BEGIN(WSC_SECURITY_PROVIDER_HEALTH)
ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_GOOD, "good")
ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED, "not_monitored")
ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_POOR, "poor")
ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_SNOOZE, "snooze")
IMPLEMENT_ENUM_END("wsc_health")

class CLocalizationDummy : public CLocalizationInterface
{
public:
    virtual SString Translate(const SString& strMessage) { return strMessage; }
    virtual SString TranslateWithContext(const SString& strContext, const SString& strMessage) { return strMessage; }
    virtual SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum) { return strPlural; }
    virtual SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum) { return strPlural; }

    virtual std::vector<SString> GetAvailableLocales() { return std::vector<SString>(); }
    virtual bool                 IsLocalized() { return false; }
    virtual SString              GetLanguageDirectory(CLanguage* pLanguage = nullptr) { return ""; }
    virtual SString              GetLanguageCode() { return "en_US"; }
    virtual SString              GetLanguageName() { return "English"; }
};

CLocalizationInterface* g_pLocalization = new CLocalizationDummy();

//////////////////////////////////////////////////////////
//
// InitLocalization
//
// Start localization thingmy
//
//////////////////////////////////////////////////////////
void InitLocalization(bool bShowErrors)
{
    static bool bDone = false;
    if (bDone)
        return;

    // Check for and load core.dll for localization
    // Use launch relative path so core.dll can get updated
    SString strCoreDLL = PathJoin(GetLaunchPath(), "GninE", GninE_DLL_NAME);
    if (!FileExists(strCoreDLL))
    {
        if (!bShowErrors)
            return;
        DisplayErrorMessageBox(("Load failed.  Please ensure that "
                                "the file " GninE_DLL_NAME " is in the modules "
                                "directory within the GninE root directory."),
                               _E("CL23"), "core-missing");            // Core.dll missing

        return ExitProcess(EXIT_ERROR);
    }

    // Use registry setting of GninE path for dlls, as they will not be present in update files
    const SString strGninESAPath = GetGninESAPath();
    SetDllDirectory(PathJoin(strGninESAPath, "GninE"));

    DWORD dwPrevMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    // See if xinput is loadable (core.dll dependency)
    for (const SString& strModuleName : std::vector<SString>({"XInput9_1_0", "xinput1_3"}))
    {
        HMODULE hXInputModule = LoadLibrary(strModuleName + ".dll");
        if (hXInputModule)
        {
            FreeLibrary(hXInputModule);
        }
        else
        {
            // If xinput is not loadable, do hack to use dll supplied with GninE
            SString strDest = PathJoin(strGninESAPath, "GninE", strModuleName + ".dll");
            if (!FileExists(strDest))
            {
                SString strSrc = PathJoin(strGninESAPath, "GninE", strModuleName + "_GninE.dll");
                if (!FileExists(strSrc))
                {
                    // dll might only exist in launch directory during auto-update
                    strSrc = PathJoin(GetLaunchPath(), "GninE", strModuleName + "_GninE.dll");
                }
                FileCopy(strSrc, strDest);
            }
        }
    }

    // Check if the core can be loaded - failure may mean d3dx9_40.dll etc is not installed
    // Use LOAD_WITH_ALTERED_SEARCH_PATH so the strCoreDLL path is searched first for dependent dlls
    if (bShowErrors)
        SetErrorMode(dwPrevMode);
    HMODULE hCoreModule = LoadLibraryEx(strCoreDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    SetErrorMode(dwPrevMode);
    if (hCoreModule == NULL)
    {
        if (!bShowErrors)
            return;
#ifdef GninE_DEBUG
        DisplayErrorMessageBox(("Loading core failed.  Please ensure that \n"
                                "the latest DirectX is correctly installed and you executed win-install-data.bat"),
                               _E("CL24"), "core-not-loadable");
#else
        DisplayErrorMessageBox(("Loading core failed.  Please ensure that \n"
                                "the latest DirectX is correctly installed."),
                               _E("CL24"), "core-not-loadable");
#endif
        return ExitProcess(EXIT_ERROR);
    }

    // Grab our locale from the registry if possible, if not Windows
    SString strLocale = GetApplicationSetting("locale");
    if (strLocale.empty())
    {
        setlocale(LC_ALL, "");
        char* szLocale = setlocale(LC_ALL, NULL);
        strLocale = szLocale;
    }

    typedef CLocalizationInterface*(__cdecl * FUNC_CREATELOCALIZATIONFROMENVIRONMENT)(SString strLocale);
    FUNC_CREATELOCALIZATIONFROMENVIRONMENT pFunc = (FUNC_CREATELOCALIZATIONFROMENVIRONMENT)GetProcAddress(hCoreModule, "L10n_CreateLocalization");
    CLocalizationInterface*                pLocalization = pFunc(strLocale);
    if (pLocalization == NULL)
    {
        if (!bShowErrors)
            return;

        DisplayErrorMessageBox(("Loading localization failed.  Please ensure that \n"
                                "the latest DirectX is correctly installed."),
                               _E("CL26"), "localization-not-loadable");
        FreeLibrary(hCoreModule);
        return ExitProcess(EXIT_ERROR);
    }

    SAFE_DELETE(g_pLocalization);
    g_pLocalization = pLocalization;
    bDone = true;

#ifdef GninE_DEBUG
    TestDialogs();
#endif
}

//////////////////////////////////////////////////////////
//
// HandleSpecialLaunchOptions
//
// Check and handle commands (from the installer)
//
//////////////////////////////////////////////////////////
void HandleSpecialLaunchOptions()
{
    // Handle service install request from the installer
    if (CommandLineContains("/kdinstall"))
    {
        UpdateGninEVersionApplicationSetting(true);
        WatchDogReset();
        WatchDogBeginSection(WD_SECTION_POST_INSTALL);
        if (CheckService(CHECK_SERVICE_POST_INSTALL))
            return ExitProcess(EXIT_OK);
        return ExitProcess(EXIT_ERROR);
    }

    // Handle service uninstall request from the installer
    if (CommandLineContains("/kduninstall"))
    {
        UpdateGninEVersionApplicationSetting(true);
        if (CheckService(CHECK_SERVICE_PRE_UNINSTALL))
            return ExitProcess(EXIT_OK);
        return ExitProcess(EXIT_ERROR);
    }

    // No run 4 sure check
    if (CommandLineContains("/nolaunch"))
    {
        return ExitProcess(EXIT_OK);
    }
}

//////////////////////////////////////////////////////////
//
// HandleDuplicateLaunching
//
// Handle duplicate launching, or running from GninEsa:// URI ?
//
//////////////////////////////////////////////////////////
void HandleDuplicateLaunching()
{
    LPSTR lpCmdLine = GetCommandLine();

    int iRecheckTimeLimit = 2000;
    while (!CreateSingleInstanceMutex())
    {
        if (strcmp(lpCmdLine, "") != 0)
        {
            HWND hwGninEWindow = FindWindow(NULL, "GninE: San Andreas");
#ifdef GninE_DEBUG
            if (hwGninEWindow == NULL)
                hwGninEWindow = FindWindow(NULL, "GninE: San Andreas [DEBUG]");
#endif
            if (hwGninEWindow != NULL)
            {
                LPWSTR  szCommandLine = GetCommandLineW();
                int     numArgs;
                LPWSTR* aCommandLineArgs = CommandLineToArgvW(szCommandLine, &numArgs);
                for (int i = 1; i < numArgs; ++i)
                {
                    if (WStringX(aCommandLineArgs[i]).BeginsWith(L"GninEsa://"))
                    {
                        WString wideConnectInfo = aCommandLineArgs[i];
                        SString strConnectInfo = ToUTF8(wideConnectInfo);

                        COPYDATASTRUCT cdStruct;
                        cdStruct.cbData = strConnectInfo.length() + 1;
                        cdStruct.lpData = const_cast<char*>(strConnectInfo.c_str());
                        cdStruct.dwData = URI_CONNECT;

                        SendMessage(hwGninEWindow, WM_COPYDATA, NULL, (LPARAM)&cdStruct);
                        break;
                    }
                }
            }
            else
            {
                if (iRecheckTimeLimit > 0)
                {
                    // Sleep a little bit and check the mutex again
                    Sleep(500);
                    iRecheckTimeLimit -= 500;
                    continue;
                }
                SString strMessage;
                strMessage +=
                    _("Trouble restarting GninE:SA\n\n"
                      "If the problem persists, open Task Manager and\n"
                      "stop the 'gta_sa.exe' and 'GninE.exe' processes\n\n\n"
                      "Try to launch GninE:SA again?");
                if (MessageBoxUTF8(0, strMessage, _("Error") + _E("CL04"), MB_ICONWARNING | MB_YESNO | MB_TOPMOST) ==
                    IDYES)            // Trouble restarting GninE:SA
                {
                    TerminateGTAIfRunning();
                    TerminateOtherGninEIfRunning();
                    ShellExecuteNonBlocking("open", PathJoin(GetGninESAPath(), GninE_EXE_NAME), lpCmdLine);
                }
                return ExitProcess(EXIT_ERROR);
            }
        }
        else
        {
            if (!IsGTARunning() && !IsOtherGninERunning())
            {
                MessageBoxUTF8(0, _("Another instance of GninE is already running.\n\nIf this problem persists, please restart your computer"),
                               _("Error") + _E("CL05"), MB_ICONERROR | MB_TOPMOST);
            }
            else if (MessageBoxUTF8(0, _("Another instance of GninE is already running.\n\nDo you want to terminate it?"), _("Error") + _E("CL06"),
                                    MB_ICONQUESTION | MB_YESNO | MB_TOPMOST) == IDYES)
            {
                TerminateGTAIfRunning();
                TerminateOtherGninEIfRunning();
                ShellExecuteNonBlocking("open", PathJoin(GetGninESAPath(), GninE_EXE_NAME), lpCmdLine);
            }
        }
        return ExitProcess(EXIT_ERROR);
    }
}

//////////////////////////////////////////////////////////
//
// HandleTrouble
//
//
//
//////////////////////////////////////////////////////////
void HandleTrouble()
{
    if (CheckAndShowFileOpenFailureMessage())
        return;

#if !defined(GninE_DEBUG) && GninESA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running GninE:SA?.\n\nDo you want to revert to an earlier version?"),
                                   "GninE: San Andreas" + _E("CL07"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
    if (iResponse == IDYES)
    {
        BrowseToSolution("crashing-before-gtagame", TERMINATE_PROCESS);
    }
#endif
}

//////////////////////////////////////////////////////////
//
// HandleResetSettings
//
//
//
//////////////////////////////////////////////////////////
void HandleResetSettings()
{
    if (CheckAndShowFileOpenFailureMessage())
        return;

    CheckAndShowMissingFileMessage();

    SString strSaveFilePath = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files");
    SString strSettingsFilename = PathJoin(strSaveFilePath, "gta_sa.set");
    SString strSettingsFilenameBak = PathJoin(strSaveFilePath, "gta_sa_old.set");

    if (FileExists(strSettingsFilename))
    {
        int iResponse = MessageBoxUTF8(NULL,
                                       _("There seems to be a problem launching GninE:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want "
                                         "to reset GTA settings now?"),
                                       "GninE: San Andreas" + _E("CL08"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES)
        {
            FileDelete(strSettingsFilenameBak);
            FileRename(strSettingsFilename, strSettingsFilenameBak);
            FileDelete(strSettingsFilename);

            // Also reset NVidia Optimus "remember option" to allow them to choose again
            SetApplicationSettingInt("nvhacks", "optimus-remember-option", 0);

            if (!FileExists(strSettingsFilename))
            {
                AddReportLog(4053, "Deleted gta_sa.set");
                MessageBoxUTF8(NULL, _("GTA settings have been reset.\n\nPress OK to continue."), "GninE: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            }
            else
            {
                AddReportLog(5054, SString("Delete gta_sa.set failed with '%s'", *strSettingsFilename));
                MessageBoxUTF8(NULL, SString(_("File could not be deleted: '%s'"), *strSettingsFilename), "Error" + _E("CL09"),
                               MB_OK | MB_ICONWARNING | MB_TOPMOST);
            }
        }
    }
    else
    {
        // No settings to delete, or can't find them
        int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running GninE:SA?.\n\nDo you want to see some online help?"), "GninE: San Andreas",
                                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES)
        {
            BrowseToSolution("crashing-before-gtalaunch", TERMINATE_PROCESS);
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleNotUsedMainMenu
//
// Called when a problem occured before the main menu was used by user
// If fullscreen, then maybe change fullscreen mode
//
//////////////////////////////////////////////////////////
void HandleNotUsedMainMenu()
{
    AddReportLog(9310, "Loader - HandleNotUsedMainMenu");
    {
        // Slighty hacky way of checking in-game settings
        SString strCoreConfigFilename = CalcGninESAPath(PathJoin("GninE", "config", "coreconfig.xml"));
        SString strCoreConfig;
        FileLoad(strCoreConfigFilename, strCoreConfig);
        SString strWindowed = strCoreConfig.SplitRight("<display_windowed>").Left(1);
        SString strFullscreenStyle = strCoreConfig.SplitRight("<display_fullscreen_style>").Left(1);
        if (strFullscreenStyle == "1")
        {
            AddReportLog(9315, "Loader - HandleNotUsedMainMenu - Already Borderless window");
        }
        else if (!strWindowed.empty() && !strFullscreenStyle.empty())
        {
            if (strWindowed == "0" && strFullscreenStyle == "0")            // 0=FULLSCREEN_STANDARD
            {
                // Inform user
                SString strMessage = _("Are you having problems running GninE:SA?.\n\nDo you want to change the following setting?");
                strMessage += "\n" + _("Fullscreen mode:") + " -> " + _("Borderless window");
                HideSplash();
                int iResponse = MessageBoxUTF8(NULL, strMessage, "GninE: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
                if (iResponse == IDYES)
                {
                    // Very hacky way of changing in-game settings
                    strCoreConfig = strCoreConfig.Replace("<display_fullscreen_style>0", "<display_fullscreen_style>1");
                    FileSave(strCoreConfigFilename, strCoreConfig);
                    AddReportLog(9311, "Loader - HandleNotUsedMainMenu - User change to Borderless window");
                }
                else
                    AddReportLog(9313, "Loader - HandleNotUsedMainMenu - User said no");
            }
            else
                AddReportLog(9314, "Loader - HandleNotUsedMainMenu - Mode not fullscreen standard");
        }
        else
        {
            // If no valid settings file yet, do the change without asking
            strCoreConfig = "<mainconfig><settings><display_fullscreen_style>1</display_fullscreen_style></settings></mainconfig>";
            FileSave(strCoreConfigFilename, strCoreConfig);
            AddReportLog(9312, "Loader - HandleNotUsedMainMenu - Set Borderless window");
        }
    }

    // Check if problem processes are active
    struct
    {
        std::vector<SString> matchTextList;
        const char*          szProductName;
        const char*          szTrouble;
    } procItems[] = {{{"\\Evolve"}, "Evolve", "not-used-menu-evolve"},
                     {{"\\GbpSv.exe", "Diebold\\Warsaw"}, "GAS Tecnologia - G-Buster Browser Defense", "not-used-menu-gbpsv"}};
    for (uint i = 0; i < NUMELMS(procItems); i++)
    {
        for (auto processId : MyEnumProcesses(true))
        {
            SString strProcessFilename = GetProcessPathFilename(processId);
            for (auto strMatchText : procItems[i].matchTextList)
            {
                if (strProcessFilename.ContainsI(strMatchText))
                {
                    SString strMessage = _("Are you having problems running GninE:SA?.\n\nTry disabling the following products for GTA and GninE:");
                    strMessage += "\n\n";
                    strMessage += procItems[i].szProductName;
                    DisplayErrorMessageBox(strMessage, _E("CL43"), procItems[i].szTrouble);
                    goto done;
                }
            }
        }
    done:;
    }
}

//////////////////////////////////////////////////////////
//
// HandleCustomStartMessage
//
//
//
//////////////////////////////////////////////////////////
void HandleCustomStartMessage()
{
    SString strStartMessage = GetApplicationSetting("diagnostics", "start-message");
    SString strTrouble = GetApplicationSetting("diagnostics", "start-message-trouble");

    if (strStartMessage.empty())
        return;

    SetApplicationSetting("diagnostics", "start-message", "");
    SetApplicationSetting("diagnostics", "start-message-trouble", "");

    if (strStartMessage.BeginsWith("vdetect"))
    {
        SString strFilename = strStartMessage.SplitRight("name=");
        strStartMessage =
            _("WARNING\n\n"
              "GninE:SA has detected unusual activity.\n"
              "Please run a virus scan to ensure your system is secure.\n\n");
        strStartMessage += SString(_("The detected file was:  %s\n"), *strFilename);
    }

    DisplayErrorMessageBox(strStartMessage, _E("CL37"), strTrouble);
}

//////////////////////////////////////////////////////////
//
// PreLaunchWatchDogs
//
//
//
//////////////////////////////////////////////////////////
void PreLaunchWatchDogs()
{
    assert(!CreateSingleInstanceMutex());

    //
    // "L0" is opened before the launch sequence and is closed if GninE shutsdown with no error
    // "L1" is opened before the launch sequence and is closed if GTA is succesfully started
    // "CR1" is a counter which is incremented if GTA was not started and GninE shutsdown with an error
    //
    // "L2" is opened before the launch sequence and is closed if the GTA loading screen is shown
    // "CR2" is a counter which is incremented at startup, if the previous run didn't make it to the loading screen
    //
    // "L3" is opened before the launch sequence and is closed if the GTA loading screen is shown, or a startup problem is handled elsewhere
    //

    // Check for unclean stop on previous run
#ifndef GninE_DEBUG
    if (WatchDogIsSectionOpen("L0"))
    {
        WatchDogSetUncleanStop(true);            // Flag to maybe do things differently if GninE exit code on last run was not 0
        CheckAndShowFileOpenFailureMessage();
    }
    else
#endif
        WatchDogSetUncleanStop(false);

    SString strCrashFlagFilename = CalcGninESAPath("GninE\\core.log.flag");
    if (FileExists(strCrashFlagFilename))
    {
        FileDelete(strCrashFlagFilename);
        WatchDogSetLastRunCrash(true);            // Flag to maybe do things differently if GninE crashed last run
    }
    else
        WatchDogSetLastRunCrash(false);

    // Reset counter if gta game was run last time
    if (!WatchDogIsSectionOpen("L1"))
        WatchDogClearCounter("CR1");

    // If crashed 3 times in a row before starting the game, do something
    if (WatchDogGetCounter("CR1") >= 3)
    {
        WatchDogReset();
        HandleTrouble();
    }

    // Check for possible gta_sa.set problems
    if (WatchDogIsSectionOpen("L2"))
    {
        WatchDogIncCounter("CR2");            // Did not reach loading screen last time
        WatchDogCompletedSection("L2");
    }
    else
        WatchDogClearCounter("CR2");

    // If didn't reach loading screen 5 times in a row, do something
    if (WatchDogGetCounter("CR2") >= 5)
    {
        WatchDogClearCounter("CR2");
        HandleResetSettings();
    }

    // Check for possible fullscreen problems
    if (WatchDogIsSectionOpen(WD_SECTION_NOT_USED_MAIN_MENU))
    {
        int iChainLimit;
        if (WatchDogIsSectionOpen(WD_SECTION_POST_INSTALL))
            iChainLimit = 1;
        else if (GetApplicationSettingInt("times-connected") == 0)
            iChainLimit = 2;
        else
            iChainLimit = 3;
        WatchDogCompletedSection(WD_SECTION_NOT_USED_MAIN_MENU);
        WatchDogIncCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
        if (WatchDogGetCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU) >= iChainLimit)
        {
            WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
            HandleNotUsedMainMenu();
        }
    }
    else
        WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);

    // Clear down freeze on quit detection
    WatchDogCompletedSection("Q0");

    WatchDogBeginSection("L0");            // Gets closed if GninE exits with a return code of 0
    WatchDogBeginSection("L1");            // Gets closed when online game has started
    SetApplicationSetting("diagnostics", "gta-fopen-fail", "");
    SetApplicationSetting("diagnostics", "last-crash-reason", "");
    SetApplicationSetting("diagnostics", "gta-fopen-last", "");
}

//////////////////////////////////////////////////////////
//
// PostRunWatchDogs
//
//
//
//////////////////////////////////////////////////////////
void PostRunWatchDogs(int iReturnCode)
{
    if (iReturnCode == 0)
    {
        WatchDogClearCounter("CR1");
        WatchDogCompletedSection("L0");
    }
}

//////////////////////////////////////////////////////////
//
// HandleIfGTAIsAlreadyRunning
//
// Check for and maybe stop a running GTA process
//
//////////////////////////////////////////////////////////
void HandleIfGTAIsAlreadyRunning()
{
    if (IsGTARunning())
    {
        if (MessageBoxUTF8(
                0, _("An instance of GTA: San Andreas is already running. It needs to be terminated before GninE:SA can be started. Do you want to do that now?"),
                _("Information") + _E("CL10"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
        {
            TerminateOtherGninEIfRunning();
            TerminateGTAIfRunning();
            if (IsGTARunning())
            {
                MessageBoxUTF8(0, _("Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer."),
                               _("Information") + _E("CL11"), MB_OK | MB_ICONERROR | MB_TOPMOST);
                return ExitProcess(EXIT_ERROR);
            }
        }
        else
            return ExitProcess(EXIT_OK);
    }
}

//////////////////////////////////////////////////////////
//
// ValidateGTAPath
//
// Check GTA path looks good
//
//////////////////////////////////////////////////////////
void ValidateGTAPath()
{
    // Get path to GTA
    ePathResult iResult = DiscoverGTAPath(true);
    if (iResult == GAME_PATH_MISSING)
    {
        DisplayErrorMessageBox(_("Registry entries are missing. Please reinstall GninE: San Andreas."), _E("CL12"), "reg-entries-missing");
        return ExitProcess(EXIT_ERROR);
    }
    else if (iResult == GAME_PATH_UNICODE_CHARS)
    {
        DisplayErrorMessageBox(
            _("The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas "
              "installation to a compatible path that contains only standard ASCII characters and reinstall GninE: San Andreas."),
            _E("CL13"));
        return ExitProcess(EXIT_ERROR);
    }

    const SString strGTAPath = GetGTAPath();
    const SString strGninESAPath = GetGninESAPath();
    if (strGTAPath.Contains(";") || strGninESAPath.Contains(";"))
    {
        DisplayErrorMessageBox(_("The path to your installation of 'GninE:SA' or 'GTA: San Andreas'\n"
                                 "contains a ';' (semicolon).\n\n"
                                 " If you experience problems when running GninE:SA,\n"
                                 " move your installation(s) to a path that does not contain a semicolon."),
                               _E("CL15"), "path-semicolon");
    }
}

//////////////////////////////////////////////////////////
//
// CheckAntiVirusStatus
//
// Maybe warn user if no anti-virus running
//
//////////////////////////////////////////////////////////
void CheckAntiVirusStatus()
{
    // Get data from WMI
    std::vector<SString> enabledList;
    std::vector<SString> disabledList;
    GetWMIAntiVirusStatus(enabledList, disabledList);

    // Get status from WSC
    WSC_SECURITY_PROVIDER_HEALTH health = (WSC_SECURITY_PROVIDER_HEALTH)-1;
    {
        using FunctionT = decltype(&WscGetSecurityProviderHealth);

        static auto _WscGetSecurityProviderHealth = ([]() -> FunctionT {
            if (HMODULE wscapi = LoadLibraryW(L"Wscapi.dll"))
            {
                return reinterpret_cast<FunctionT>(static_cast<void*>(GetProcAddress(wscapi, "WscGetSecurityProviderHealth")));
            }

            return nullptr;
        })();

        if (_WscGetSecurityProviderHealth)
            _WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &health);
    }

    // Dump results
    SString strStatus("AV health: %s (%d)", *EnumToString(health), health);
    for (uint i = 0; i < enabledList.size(); i++)
        strStatus += SString(" [Ena%d:%s]", i, *enabledList[i]);
    for (uint i = 0; i < disabledList.size(); i++)
        strStatus += SString(" [Dis%d:%s]", i, *disabledList[i]);
    WriteDebugEvent(strStatus);

    // Maybe show dialog if av not found
    if (enabledList.empty() && health != WSC_SECURITY_PROVIDER_HEALTH_GOOD)
    {
        bool bEnableScaremongering = (health != WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED);

        if (bEnableScaremongering)
        {
            const char* avProducts[] = {
                "antivirus", "anti-virus", "Avast",  "AVG",    "Avira",    "NOD32", "ESET",        "F-Secure",
                "Faronics",  "Kaspersky",  "McAfee", "Norton", "Symantec", "Panda", "Trend Micro",
            };

            // Check for anti-virus helper dlls before actual scaremongering
            HMODULE aModules[1024];
            DWORD   cbNeeded;
            if (EnumProcessModules(GetCurrentProcess(), aModules, sizeof(aModules), &cbNeeded))
            {
                DWORD cModules = cbNeeded / sizeof(HMODULE);
                for (uint i = 0; i < cModules; i++)
                {
                    if (aModules[i] != 0)
                    {
                        WCHAR szModulePathFileName[1024] = L"";
                        GetModuleFileNameExW(GetCurrentProcess(), aModules[i], szModulePathFileName, NUMELMS(szModulePathFileName));
                        SLibVersionInfo libVersionInfo;
                        GetLibVersionInfo(ToUTF8(szModulePathFileName), &libVersionInfo);

                        for (uint i = 0; i < NUMELMS(avProducts); i++)
                        {
                            if (libVersionInfo.strCompanyName.ContainsI(avProducts[i]) || libVersionInfo.strProductName.ContainsI(avProducts[i]))
                            {
                                bEnableScaremongering = false;
                                WriteDebugEvent(SString("AV (module) maybe found %s [%d](%s,%s)", *WStringX(szModulePathFileName).ToAnsi(), i,
                                                        *libVersionInfo.strCompanyName, *libVersionInfo.strProductName));
                            }
                        }
                    }
                }

                if (bEnableScaremongering)
                    WriteDebugEvent(SString("AV Searched %d dlls, but could not find av helper", cModules));
            }

            if (bEnableScaremongering)
            {
                for (auto processId : MyEnumProcesses(true))
                {
                    SString strProcessPathFileName = GetProcessPathFilename(processId);
                    if (!strProcessPathFileName.empty())
                    {
                        SLibVersionInfo libVersionInfo;
                        if (GetLibVersionInfo(strProcessPathFileName, &libVersionInfo))
                        {
                            for (uint i = 0; i < NUMELMS(avProducts); i++)
                            {
                                if (libVersionInfo.strCompanyName.ContainsI(avProducts[i]) || libVersionInfo.strProductName.ContainsI(avProducts[i]))
                                {
                                    bEnableScaremongering = false;
                                    WriteDebugEvent(SString("AV (process) maybe found %s [%d](%s,%s)", *strProcessPathFileName, i,
                                                            *libVersionInfo.strCompanyName, *libVersionInfo.strProductName));
                                }
                            }
                        }
                    }
                }
                if (bEnableScaremongering)
                    WriteDebugEvent("AV Searched processes, but could not find av helper");
            }
        }

        ShowNoAvDialog(g_hInstance, bEnableScaremongering);
        HideNoAvDialog();
    }
}

//////////////////////////////////////////////////////////
//
// CheckDataFiles
//
// Basic check for some essential files
//
//////////////////////////////////////////////////////////
void CheckDataFiles()
{
    const SString strGninESAPath = GetGninESAPath();
    const SString strGTAPath = GetGTAPath();

    const char* dataFilesFiles[] = {"GninE\\cgui\\images\\background_logo.png",
                                    "GninE\\cgui\\images\\radarset\\up.png",
                                    "GninE\\cgui\\images\\busy_spinner.png",
                                    "GninE\\data\\gta_sa_diff.dat",
                                    "GninE\\D3DX9_42.dll",
                                    "GninE\\D3DCompiler_42.dll",
                                    "GninE\\d3dcompiler_43.dll",
                                    "GninE\\d3dcompiler_47.dll",
                                    "GninE\\bass.dll",
                                    "GninE\\bass_ac3.dll",
                                    "GninE\\bassflac.dll",
                                    "GninE\\bassmix.dll",
                                    "GninE\\basswebm.dll",
                                    "GninE\\bass_aac.dll",
                                    "GninE\\bass_fx.dll",
                                    "GninE\\bassmidi.dll",
                                    "GninE\\bassopus.dll",
                                    "GninE\\basswma.dll",
                                    "GninE\\tags.dll",
                                    "GninE\\sa.dat",
                                    "GninE\\xinput1_3_GninE.dll",
                                    "GninE\\XInput9_1_0_GninE.dll"};

    for (uint i = 0; i < NUMELMS(dataFilesFiles); i++)
    {
        if (!FileExists(PathJoin(strGninESAPath, dataFilesFiles[i])))
        {
            DisplayErrorMessageBox(_("Load failed. Please ensure that the latest data files have been installed correctly."), _E("CL16"),
                                   "GninE-datafiles-missing");
            return ExitProcess(EXIT_ERROR);
        }
    }

    // Check for client file
    if (!FileExists(PathJoin(strGninESAPath, CHECK_DM_CLIENT_NAME)))
    {
        DisplayErrorMessageBox(SString(_("Load failed. Please ensure that %s is installed correctly."), CHECK_DM_CLIENT_NAME), _E("CL18"), "client-missing");
        return ExitProcess(EXIT_ERROR);
    }

    // Make sure the gta executable exists
    if (!FileExists(PathJoin(strGTAPath, GTA_EXE_NAME)) && !FileExists(PathJoin(strGTAPath, STEAM_GTA_EXE_NAME)))
    {
        DisplayErrorMessageBox(SString(_("Load failed. Could not find gta_sa.exe in %s."), strGTAPath.c_str()), _E("CL20"), "gta_sa-missing");
        return ExitProcess(EXIT_ERROR);
    }

    // Make sure important dll's do not exist in the wrong place
    const char* dllCheckList[] = {"xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll", "pthread.dll"};
    for (int i = 0; i < NUMELMS(dllCheckList); i++)
    {
        if (FileExists(PathJoin(strGTAPath, dllCheckList[i])))
        {
            DisplayErrorMessageBox(SString(_("Load failed. %s exists in the GTA directory. Please delete before continuing."), dllCheckList[i]), _E("CL21"),
                                   "file-clash");
            return ExitProcess(EXIT_ERROR);
        }
    }

    // Check main exe has the correct name
    if (GetLaunchFilename().CompareI(GninE_EXE_NAME) == false)
    {
        SString strMessage(_("Main file has an incorrect name (%s)"), *GetLaunchFilename());
        int     iResponse = MessageBoxUTF8(NULL, strMessage, _("Error") + _E("CL33"), MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST);
        ReleaseSingleInstanceMutex();
        if (iResponse == IDRETRY)
            ShellExecuteNonBlocking("open", PathJoin(strGninESAPath, GninE_EXE_NAME));
        return ExitProcess(EXIT_ERROR);
    }

    // Check for possible virus file changing activities
    if (!VerifyEmbeddedSignature(PathJoin(strGninESAPath, GninE_EXE_NAME)))
    {
        SString strMessage(_("Main file is unsigned. Possible virus activity.\n\nSee online help if GninE does not work correctly."));
        #if GninESA_VERSION_BUILD > 0 && defined(GninE_DM_PUBLIC_CONNECTIONS) && !defined(GninE_DEBUG)
        DisplayErrorMessageBox(strMessage, _E("CL29"), "maybe-virus1");
        #endif
    }

    struct
    {
        const char* expected;
        const char* fileName;
    } integrityCheckList[] = {{"DFACEEEB636240C0190C7B1B0145C89E", "bass.dll"},           {"FBE6A48C4E9FAAA6569C360DD49CCE06", "bass_aac.dll"},
                              {"E7E69A3B369F0ABA1A4A18C831BC4364", "bass_ac3.dll"},       {"8164042444F819CA107297CA4A0408F3", "bass_fx.dll"},
                              {"9F48DD702AB5BE002F9223E3B45A2261", "bassflac.dll"},       {"4E89426025D1E1F524495D910B60C709", "bassmidi.dll"},
                              {"5DEEC10A943E352EF7E0223327E8B48C", "bassmix.dll"},        {"2F87C5E0A1B7B28C8FC0D7E74116DDFC", "bassopus.dll"},
                              {"0F1B2FC6C0C703A43A24DC05352E7ADA", "basswebm.dll"},       {"893113C6C49DC1E1EF288310E68DB306", "basswma.dll"},
                              {"C6A44FC3CF2F5801561804272217B14D", "D3DX9_42.dll"},  {"D439E8EDD8C93D7ADE9C04BCFE9197C6", "sa.dat"},
                              {"B33B21DB610116262D906305CE65C354", "D3DCompiler_42.dll"}, {"02ECD2919B3DAA59D6014EEFD29FC294", "tags.dll"},
                              {"0B3DD892007FB366D1F52F2247C046F5", "d3dcompiler_43.dll"}, {"D5D8C8561C6DDA7EF0D7D6ABB0D772F4", "xinput1_3_GninE.dll"},
                              /*{"7096EB0458485D89BB749474550C7651", "d3dcompiler_47.dll"},*/ // MTA Version!
                              {"3B4647BCB9FEB591C2C05D1A606ED988", "d3dcompiler_47.dll"}, {"E2BAC93166B0C2AD4D83E97FC1E88F8F", "XInput9_1_0_GninE.dll"}};

    for (const auto& item : integrityCheckList)
    {
        SString filePath = PathJoin(strGninESAPath, "GninE", item.fileName);

        if (!FileExists(filePath))
        {
            SString message(_("Data file %s is missing. Possible virus activity.\n\nConsider reinstalling GninE for your security.\nSee online "
                              "help if GninE does not work correctly."),
                            item.fileName);
            DisplayErrorMessageBox(message, _E("CL30"), "maybe-virus2");
            break;
        }

        SString computed = CMD5Hasher::CalculateHexString(filePath);

        if (!computed.CompareI(item.expected))
        {
            SString message(_("Data file %s is modified. Possible virus activity.\n\nConsider reinstalling GninE for your security.\nSee online "
                              "help if GninE does not work correctly."),
                            item.fileName);
            DisplayErrorMessageBox(message, _E("CL30"), "maybe-virus2");
            break;
        }
    }

    // Check for asi files
    {
        bool bFoundInGTADir = !FindFiles(PathJoin(strGTAPath, "*.asi"), true, false).empty();
        bool bFoundInGninEDir = !FindFiles(PathJoin(strGninESAPath, "GninE", "*.asi"), true, false).empty();
        if (bFoundInGTADir || bFoundInGninEDir)
        {
            DisplayErrorMessageBox(_(".asi files are in the 'GninE:SA' or 'GTA: San Andreas' installation directory.\n\n"
                                     "Remove these .asi files if you experience problems with GninE:SA."),
                                   _E("CL28"), "asi-files");
        }
    }

    // Check for graphics libraries in the GTA/GninE install directory
    {
        // An array of pairs of: a registry prefix and a directory path
        std::array<std::pair<const char*, SString>, 2> directoriesToCheck = {{{"", strGTAPath}, {"GninE-", PathJoin(strGninESAPath, "GninE")}}};

        std::vector<GraphicsLibrary> offenders;

        for (const std::pair<const char*, SString>& directory : directoriesToCheck)
        {
            for (const char* libraryName : {"d3d9", "dxgi"})
            {
                GraphicsLibrary library(libraryName);
                library.absoluteFilePath = PathJoin(directory.second, library.stem + ".dll");

                if (!FileExists(library.absoluteFilePath))
                    continue;

                library.appLastHash = SString("%s%s-dll-last-hash", directory.first, library.stem.c_str());
                library.appDontRemind = SString("%s%s-dll-not-again", directory.first, library.stem.c_str());
                library.md5Hash = CMD5Hasher::CalculateHexString(library.absoluteFilePath);
                WriteDebugEvent(SString("Detected graphics library %s (md5: %s)", library.absoluteFilePath.c_str(), library.md5Hash.c_str()));

                bool isProblematic = true;

                if (GetApplicationSetting("diagnostics", library.appLastHash) == library.md5Hash)
                {
                    if (GetApplicationSetting("diagnostics", library.appDontRemind) == "yes")
                    {
                        isProblematic = false;
                    }
                }

                if (isProblematic)
                {
                    offenders.emplace_back(std::move(library));
                }
            }
        }

        if (!offenders.empty())
        {
            ShowGraphicsDllDialog(g_hInstance, offenders);
            HideGraphicsDllDialog();
        }
    }

    // Remove old log files saved in the wrong place
    SString strMtaDir = PathJoin(strGninESAPath, "GninE");
    if (strGTAPath.CompareI(strMtaDir) == false)
    {
        FileDelete(PathJoin(strGTAPath, "CEGUI.log"));
        FileDelete(PathJoin(strGTAPath, "logfile.txt"));
        FileDelete(PathJoin(strGTAPath, "shutdown.log"));
    }
}

//////////////////////////////////////////////////////////
//
// CheckLibVersions
//
// Ensure DLLs are the correct version
//
//////////////////////////////////////////////////////////
void CheckLibVersions()
{
#if GninESA_VERSION_TYPE == VERSION_TYPE_RELEASE

    const char* moduleList[] = {"GninE\\loader.dll",
                                "GninE\\cgui.dll",
                                "GninE\\core.dll",
                                "GninE\\game_sa.dll",
                                "GninE\\multiplayer_sa.dll",
                                "GninE\\netc.dll",
                                "GninE\\xmll.dll",
                                "GninE\\game_sa.dll",
                                "GninE\\" LOADER_PROXY_DLL_NAME,
                                "mods\\deathmatch\\client.dll",
                                "mods\\deathmatch\\pcre3.dll"};
    SString     strReqFileVersion;
    for (uint i = 0; i < NUMELMS(moduleList); i++)
    {
        SString strFilename = moduleList[i];
#ifdef GninE_DEBUG
        strFilename = ExtractBeforeExtension(strFilename) + "_d." + ExtractExtension(strFilename);
#endif
        SLibVersionInfo fileInfo;
        if (FileExists(CalcGninESAPath(strFilename)))
        {
            SString strFileVersion = "0.0.0.0";
            if (GetLibVersionInfo(CalcGninESAPath(strFilename), &fileInfo))
                strFileVersion = SString("%d.%d.%d.%d", fileInfo.dwFileVersionMS >> 16, fileInfo.dwFileVersionMS & 0xFFFF, fileInfo.dwFileVersionLS >> 16,
                                         fileInfo.dwFileVersionLS & 0xFFFF);
            if (strReqFileVersion.empty())
                strReqFileVersion = strFileVersion;
            else if (strReqFileVersion != strFileVersion)
            {
                DisplayErrorMessageBox(SStringX(_("File version mismatch error."
                                                  " Reinstall GninE:SA if you experience problems.\n") +
                                                SString("\n[%s %s/%s]\n", *strFilename, *strFileVersion, *strReqFileVersion)),
                                       _E("CL40"), "bad-file-version");
                break;
            }
        }
        else
        {
            DisplayErrorMessageBox(SStringX(_("Some files are missing."
                                              " Reinstall GninE:SA if you experience problems.\n") +
                                            SString("\n[%s]\n", *strFilename)),
                                   _E("CL41"), "missing-file");
            break;
        }
    }

#endif

    if (GetSystemMetrics(SM_CLEANBOOT) != 0)
    {
        DisplayErrorMessageBox(SStringX(_("GninE:SA is not compatible with Windows 'Safe Mode'.\n\n"
                                          "Please restart your PC.\n")),
                               _E("CL42"), "safe-mode");
        return ExitProcess(EXIT_ERROR);
    }
}

//////////////////////////////////////////////////////////
//
// StartGtaProcess
//
// Start GTA as an independent process
//
//////////////////////////////////////////////////////////
BOOL StartGtaProcess(const SString& lpApplicationName, const SString& lpCommandLine, const SString& lpCurrentDirectory,
                     LPPROCESS_INFORMATION lpProcessInformation, DWORD& dwOutError, SString& strOutErrorContext)
{
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    BOOL wasProcessCreated = CreateProcessW(*FromUTF8(lpApplicationName), FromUTF8(lpCommandLine).data(), nullptr, nullptr, FALSE, 0, nullptr,
                                            *FromUTF8(lpCurrentDirectory), &startupInfo, lpProcessInformation);

    if (wasProcessCreated)
        return true;

    std::vector<DWORD> processIdListBefore = GetGTAProcessList();

    if (!ShellExecuteNonBlocking("open", lpApplicationName, lpCommandLine, lpCurrentDirectory))
    {
        dwOutError = GetLastError();
        strOutErrorContext = "ShellExecute";
        return false;
    }

    // Determine pid of new gta process
    for (uint i = 0; i < 10; i++)
    {
        std::vector<DWORD> processIdList = GetGTAProcessList();
        for (DWORD pid : processIdList)
        {
            if (ListContains(processIdListBefore, pid))
            {
                continue;
            }
            lpProcessInformation->dwProcessId = pid;
            lpProcessInformation->hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid);
            break;
        }
        if (lpProcessInformation->dwProcessId)
            break;
        Sleep(500);
    }

    if (lpProcessInformation->dwProcessId == 0)
    {
        // Unable to get pid
        dwOutError = ERROR_INVALID_FUNCTION;
        strOutErrorContext = "FindPID";
        wasProcessCreated = false;
    }
    else if (lpProcessInformation->hProcess == nullptr)
    {
        // Unable to OpenProcess
        dwOutError = ERROR_ELEVATION_REQUIRED;
        strOutErrorContext = "OpenProcess";
        wasProcessCreated = false;
    }

    return wasProcessCreated;
}

//////////////////////////////////////////////////////////
//
// LaunchGame
//
// Create GTA process, hook it and go go go
//
//////////////////////////////////////////////////////////
int LaunchGame(SString strCmdLine)
{
    CheckAndShowModelProblems();
    CheckAndShowUpgradeProblems();
    CheckAndShowImgProblems();

    const SString strGTAPath = GetGTAPath();
    const SString strGninESAPath = GetGninESAPath();
    SString       strMtaDir = PathJoin(strGninESAPath, "GninE");
    SString       strGTAEXEPath = GetGameExecutablePath().u8string();

    SetDllDirectory(strMtaDir);
    if (!CheckService(CHECK_SERVICE_PRE_CREATE) && !IsUserAdmin())
    {
        RelaunchAsAdmin(strCmdLine, _("Fix configuration issue"));
        ExitProcess(EXIT_OK);
    }

    // Do some D3D things
    BeginD3DStuff();
    LogSettings();

    WatchDogBeginSection("L2");                                     // Gets closed when loading screen is shown
    WatchDogBeginSection("L3");                                     // Gets closed when loading screen is shown, or a startup problem is handled elsewhere
    WatchDogBeginSection(WD_SECTION_NOT_USED_MAIN_MENU);            // Gets closed when the main menu is used

    // Extract 'done-admin' flag from command line
    bool bDoneAdmin = strCmdLine.Contains("/done-admin");
    strCmdLine = strCmdLine.Replace(" /done-admin", "");

    // Add server connection after update to command line
    SString strPostUpdateConnect = GetPostUpdateConnect();
    if (!strPostUpdateConnect.empty() && strCmdLine.empty())
        strCmdLine = SString("GninEsa://%s", *strPostUpdateConnect);

    //
    // Launch GTA
    //
    PROCESS_INFORMATION piLoadee = {0};
    DWORD               dwError;
    SString             strErrorContext;
    if (FALSE == StartGtaProcess(strGTAEXEPath, strCmdLine, strGTAPath, &piLoadee, dwError, strErrorContext))
    {
        WriteDebugEvent(SString("Loader - Process not created[%d (%s)]: %s", dwError, *strErrorContext, *strGTAEXEPath));

        if (dwError == ERROR_ELEVATION_REQUIRED && !bDoneAdmin)
        {
            // Try to relaunch as admin if not done so already
            RelaunchAsAdmin(strCmdLine + " /done-admin", _("Fix elevation required error"));
            ExitProcess(EXIT_OK);
        }
        else
        {
            // Otherwise, show error message
            SString strError = GetSystemErrorMessage(dwError) + " (" + strErrorContext + ")";
            DisplayErrorMessageBox(SString(_("Could not start Grand Theft Auto: San Andreas.  "
                                             "Please try restarting, or if the problem persists,"
                                             "contact GninE at www.gnine.com. \n\n[%s]"),
                                           *strError),
                                   _E("CL22"), "createprocess-fail&err=" + strError);            // Could not start GTA:SA
            return 5;
        }
    }

    WriteDebugEvent(SString("Loader - Process created: %s %s", *strGTAEXEPath, *GetApplicationSetting("serial")));
    WriteDebugEvent(SString("Loader - Process ID: %lu, Thread ID: %lu", piLoadee.dwProcessId, piLoadee.dwThreadId));

    // Clear previous on quit commands
    SetOnQuitCommand("");

    ShowSplash(g_hInstance);            // Bring splash to the front

    if (piLoadee.hProcess)
    {
        WriteDebugEvent("Loader - Waiting for L3 to close");

        BsodDetectionOnGameBegin();
        // Show splash until game window is displayed (or max 20 seconds)
        DWORD status;
        for (uint i = 0; i < 20; i++)
        {
            status = WaitForSingleObject(piLoadee.hProcess, 1000);
            if (status != WAIT_TIMEOUT)
                break;

            if (!WatchDogIsSectionOpen("L3"))            // Gets closed when loading screen is shown
            {
                WriteDebugEvent("Loader - L3 closed");
                break;
            }

            // Keep showing splash if the device selection dialog is open
            if (IsDeviceSelectionDialogOpen(piLoadee.dwProcessId))
            {
                i--;
            }
        }

        // Actually hide the splash
        HideSplash();

        // If hasn't shown the loading screen and gta_sa.exe process memory usage is not changing, give user option to terminate
        if (status == WAIT_TIMEOUT)
        {
            CStuckProcessDetector stuckProcessDetector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("L3"))            // Gets closed when loading screen is shown
            {
                if (stuckProcessDetector.UpdateIsStuck())
                {
                    WriteDebugEvent("Detected stuck process at startup");
                    if (MessageBoxUTF8(0, _("GTA: San Andreas may not have launched correctly. Do you want to terminate it?"), _("Information") + _E("CL25"),
                                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
                    {
                        WriteDebugEvent("User selected process termination");
                        TerminateProcess(piLoadee.hProcess, 1);
                    }
                    break;
                }
                status = WaitForSingleObject(piLoadee.hProcess, 1000);
            }
        }

        // Wait for game to exit
        WriteDebugEvent("Loader - Wait for game to exit");
        while (status == WAIT_TIMEOUT)
        {
            status = WaitForSingleObject(piLoadee.hProcess, 1500);

            // If core is closing and gta_sa.exe process memory usage is not changing, terminate
            CStuckProcessDetector stuckProcessDetector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("Q0"))            // Gets closed when quit has completed
            {
                if (stuckProcessDetector.UpdateIsStuck())
                {
                    WriteDebugEvent("Detected stuck process at quit");
                #ifndef GninE_DEBUG
                    TerminateProcess(piLoadee.hProcess, 1);
                    status = WAIT_FAILED;
                    break;
                #endif
                }
                status = WaitForSingleObject(piLoadee.hProcess, 1000);
            }
        }

        BsodDetectionOnGameEnd();
    }

    AddReportLog(7104, "Loader - Finishing");
    WriteDebugEvent("Loader - Finishing");

    EndD3DStuff();

    // Get its exit code
    DWORD dwExitCode = -1;
    GetExitCodeProcess(piLoadee.hProcess, &dwExitCode);

    // Terminate to be sure
    TerminateProcess(piLoadee.hProcess, 1);

    //////////////////////////////////////////////////////////
    //
    // On exit
    //
    // Cleanup and exit.
    CloseHandle(piLoadee.hProcess);
    ReleaseSingleInstanceMutex();

    // Success, maybe
    return dwExitCode;
}

//////////////////////////////////////////////////////////
//
// HandleOnQuitCommand
//
// Parse and do something with OnQuitCommand from registry
//
//////////////////////////////////////////////////////////
void HandleOnQuitCommand()
{
    const SString strGninESAPath = GetGninESAPath();
    SetCurrentDirectory(strGninESAPath);
    SetDllDirectory(strGninESAPath);

    SString strOnQuitCommand = GetRegistryValue("", "OnQuitCommand");

    std::vector<SString> vecParts;
    strOnQuitCommand.Split("\t", vecParts);
    if (vecParts.size() > 4 && vecParts[0].length())
    {
        SString strOperation = vecParts[0];
        SString strFile = vecParts[1];
        SString strParameters = vecParts[2];
        SString strDirectory = vecParts[3];
        SString strShowCmd = vecParts[4];

        if (strOperation == "restart")
        {
            strOperation = "open";
            strFile = PathJoin(strGninESAPath, GninE_EXE_NAME);
            CheckService(CHECK_SERVICE_RESTART_GAME);
        }
        else
            CheckService(CHECK_SERVICE_POST_GAME);            // Stop service here if quit command is not 'restart'

        LPCTSTR lpOperation = strOperation == "" ? NULL : strOperation.c_str();
        LPCTSTR lpFile = strFile.c_str();
        LPCTSTR lpParameters = strParameters == "" ? NULL : strParameters.c_str();
        LPCTSTR lpDirectory = strDirectory == "" ? NULL : strDirectory.c_str();
        INT     nShowCmd = strShowCmd == "" ? SW_SHOWNORMAL : atoi(strShowCmd);

        if (lpOperation && lpFile)
        {
            // Maybe spawn an exe
            ShellExecuteNonBlocking(lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
        }
    }
    else
        CheckService(CHECK_SERVICE_POST_GAME);            // Stop service here if quit command is empty
}
