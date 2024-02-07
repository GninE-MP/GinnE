/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServerInfo.cpp
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CServerInfo.h"

CServerInfo::CServerInfo() : m_MaxPlayersCount(0)
{
}

void CServerInfo::SetMaxPlayers(uint set)
{
    m_MaxPlayersCount = set;
}
