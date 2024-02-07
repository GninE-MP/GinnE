/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDestroySatchelsPacket.h
 *  PURPOSE:     Satchel destruction packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CDestroySatchelsPacket final : public CPacket
{
public:
    CDestroySatchelsPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_DESTROY_SATCHELS; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;
};
