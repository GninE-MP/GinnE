/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPickupHitConfirmPacket.cpp
 *  PURPOSE:     Pickup hit confirmation packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPickupHitConfirmPacket.h"
#include "CPickup.h"

bool CPickupHitConfirmPacket::Write(NetBitStreamInterface& BitStream) const
{
    // unsigned short   (2)     - pickup id
    // bool                     - hide it?

    // Got a pickup to send?
    if (m_pPickup)
    {
        // Write the pickup id and visibily state
        BitStream.Write(m_pPickup->GetID());

        // WRite the flags
        BitStream.WriteBit(m_pPickup->IsVisible());
        BitStream.WriteBit(m_bPlaySound);

        return true;
    }

    return false;
}
