/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/MainFunctions.h
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#pragma once

void InitLocalization(bool bShowErrors);
void HandleSpecialLaunchOptions();
void HandleDuplicateLaunching();
void HandleCustomStartMessage();
void PreLaunchWatchDogs();
void PostRunWatchDogs(int iReturnCode);
void HandleIfGTAIsAlreadyRunning();
void ValidateGTAPath();
void CheckAntiVirusStatus();
void CheckDataFiles();
void CheckLibVersions();
int  LaunchGame(SString strCmdLine);
void HandleOnQuitCommand();

#define EXIT_OK     (0)
#define EXIT_ERROR  (1)
