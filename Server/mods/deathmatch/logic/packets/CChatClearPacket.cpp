/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CChatClearPacket.h
 *  PURPOSE:     Chat message echo packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CChatClearPacket.h"

// Needed because compiler throwing LNK2001 and LNK1120 error.
bool CChatClearPacket::Write(NetBitStreamInterface& BitStream) const
{
    return true;
}
