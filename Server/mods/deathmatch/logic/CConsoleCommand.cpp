/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsoleCommand.cpp
 *  PURPOSE:     Console command class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CConsoleCommand.h"

CConsoleCommand::CConsoleCommand(FCommandHandler* pHandler, const char* szCommand, bool bRestricted, const char* szConsoleHelpText)
{
    // Init
    m_pHandler = pHandler;

    // Copy the command name string
    m_szCommand = new char[strlen(szCommand) + 1];
    strcpy(m_szCommand, szCommand);
    m_bRestricted = bRestricted;
    m_szConsoleHelpText = szConsoleHelpText;
}

bool CConsoleCommand::operator()(CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient)
{
    return m_pHandler(pConsole, szArguments, pClient, pEchoClient);
}
