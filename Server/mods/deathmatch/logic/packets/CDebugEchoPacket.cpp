/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDebugEchoPacket.cpp
 *  PURPOSE:     Debug message echo packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDebugEchoPacket.h"

bool CDebugEchoPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the level
    BitStream.Write(static_cast<unsigned char>(m_uiLevel));
    if (m_uiLevel == 0)
    {
        // Write the color
        BitStream.Write(m_ucRed);
        BitStream.Write(m_ucGreen);
        BitStream.Write(m_ucBlue);
    }

    // Too short?
    if (m_strMessage.length() >= MIN_DEBUGECHO_LENGTH)
    {
        // Write the string
        BitStream.WriteStringCharacters(m_strMessage);
        return true;
    }

    return false;
}
