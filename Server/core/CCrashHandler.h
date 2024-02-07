/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCrashHandler.h
 *  PURPOSE:     Crash handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CCrashHandlerAPI.h"

#ifdef WIN32
    #include <windows.h>
#else
    #include <signal.h>
#endif

class CExceptionInformation;

class CCrashHandler
{
public:
    static void Init(const SString& strServerPath);

private:
    #ifdef WIN32

    static long WINAPI HandleExceptionGlobal(_EXCEPTION_POINTERS* pException);
    static void        DumpMiniDump(_EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation);

    static void RunErrorTool();

    #else

    static void HandleExceptionGlobal(int iSig);

    #endif
};
