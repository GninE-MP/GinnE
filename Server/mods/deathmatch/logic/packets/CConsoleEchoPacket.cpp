/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CConsoleEchoPacket.cpp
 *  PURPOSE:     Console message echo packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CConsoleEchoPacket.h"

bool CConsoleEchoPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Not too short?
    if (m_strMessage.length() >= MIN_CONSOLEECHO_LENGTH)
    {
        // Write the string
        BitStream.WriteStringCharacters(m_strMessage);
        return true;
    }

    return false;
}
