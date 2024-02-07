/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerTransgressionPacket.cpp
 *  PURPOSE:
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerTransgressionPacket.h"

bool CPlayerTransgressionPacket::Read(NetBitStreamInterface& BitStream)
{
    return BitStream.Read(m_uiLevel) && BitStream.ReadString(m_strMessage);
}
