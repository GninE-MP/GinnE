/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerInfoSyncPacket.cpp
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerInfoSyncPacket.h"
#include "CStaticFunctionDefinitions.h"

bool CServerInfoSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_ActualInfo)            // Flag is set
    {
        BitStream.Write(m_ActualInfo);

        // Check the flags one by one & write in order
        if (maxPlayers)
            BitStream.Write(static_cast<uint>(
                CStaticFunctionDefinitions::GetMaxPlayers()));            // static_cast ensures the type is uint in case it's changed in future

        // other info

        return true;
    }

    return false;
}
