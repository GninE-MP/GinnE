/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPacket.cpp
 *  PURPOSE:     Packet base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPacket.h"
#include "CPlayer.h"

CPacket::CPacket()
{
    // Init
    m_pSourceElement = NULL;
    m_Source = NetServerPlayerID(0, 0);
}

CPlayer* CPacket::GetSourcePlayer()
{
    return static_cast<CPlayer*>(m_pSourceElement);
}
