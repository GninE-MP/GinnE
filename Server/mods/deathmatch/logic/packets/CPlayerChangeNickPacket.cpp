/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerChangeNickPacket.cpp
 *  PURPOSE:     Player nickname change packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerChangeNickPacket.h"
#include "CElement.h"

CPlayerChangeNickPacket::CPlayerChangeNickPacket(const char* szNewNick)
{
    m_strNewNick.AssignLeft(szNewNick, MAX_PLAYER_NICK_LENGTH);
}

bool CPlayerChangeNickPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_pSourceElement)
    {
        // Write the source player id
        ElementID ID = m_pSourceElement->GetID();
        BitStream.Write(ID);

        // Write the nick
        BitStream.WriteStringCharacters(m_strNewNick);
        return true;
    }

    return false;
}
