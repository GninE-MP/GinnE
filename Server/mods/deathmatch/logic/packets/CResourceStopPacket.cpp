/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceStopPacket.cpp
 *  PURPOSE:     Resource stop packet handler class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceStopPacket.h"

CResourceStopPacket::CResourceStopPacket(unsigned short usID)
{
    m_usID = usID;
}

bool CResourceStopPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the resource id
    BitStream.Write(m_usID);
    return true;
}
