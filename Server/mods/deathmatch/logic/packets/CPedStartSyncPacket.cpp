/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedStartSyncPacket.cpp
 *  PURPOSE:     Ped start synchronization packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedStartSyncPacket.h"
#include "CPed.h"

bool CPedStartSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_pPed)
        return false;

    BitStream.Write(m_pPed->GetID());

    CVector vecTemp;

    vecTemp = m_pPed->GetPosition();
    BitStream.Write(vecTemp.fX);
    BitStream.Write(vecTemp.fY);
    BitStream.Write(vecTemp.fZ);

    BitStream.Write(m_pPed->GetRotation());

    m_pPed->GetVelocity(vecTemp);
    BitStream.Write(vecTemp.fX);
    BitStream.Write(vecTemp.fY);
    BitStream.Write(vecTemp.fZ);

    BitStream.Write(m_pPed->GetHealth());
    BitStream.Write(m_pPed->GetArmor());

    return true;
}
