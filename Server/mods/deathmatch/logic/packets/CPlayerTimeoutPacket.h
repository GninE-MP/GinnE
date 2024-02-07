/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerTimeoutPacket.h
 *  PURPOSE:     Player timeout packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerTimeoutPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return static_cast<ePacketID>(PACKET_ID_PLAYER_TIMEOUT); };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream) { return true; };
    bool Write(NetBitStreamInterface& BitStream) const { return true; };
};
