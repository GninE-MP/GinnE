/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CThreadCommandQueue.h
 *  PURPOSE:     Thread command queue class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifdef WIN32

#include <list>
#include <string>

class CThreadCommandQueue
{
public:
    void Add(const char* szCommand);
    void Process(bool& bRequestedQuit, class CModManagerImpl* pModManager);

private:
    std::list<std::string> m_Commands;
    CCriticalSection       m_Critical;
};

#endif
