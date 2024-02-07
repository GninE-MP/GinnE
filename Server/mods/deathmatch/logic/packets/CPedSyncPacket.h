/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedSyncPacket.h
 *  PURPOSE:     Ped synchronization packet class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "CPacket.h"
#include <vector>

class CPedSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        ElementID     ID;
        unsigned char ucFlags;
        unsigned char ucSyncTimeContext;
        CVector       vecPosition;
        float         fRotation;
        CVector       vecVelocity;
        float         fHealth;
        float         fArmor;
        bool          bOnFire;
        bool          bIsInWater;
    };

public:
    // Used when receiving ped sync from clients, can contain multiple SyncData
    CPedSyncPacket(){};
    // Used when sending ped sync to clients, only contains one SyncData
    CPedSyncPacket(SyncData& pReadData);

    ePacketID     GetPacketID() const { return PACKET_ID_PED_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData> m_Syncs;
};
