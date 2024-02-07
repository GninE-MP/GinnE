/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPacketTranslator.h
 *  PURPOSE:     Network packet translator class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

class CPacketTranslator;

#pragma once

#include "CCommon.h"
#include "CPlayerManager.h"
#include "packets/CPacket.h"

// Left in for dummy purposes
#include "packets/CVoiceDataPacket.h"

class CPacketTranslator
{
public:
    CPacketTranslator(CPlayerManager* pPlayerManager);
    ~CPacketTranslator();

    CPacket* Translate(const NetServerPlayerID& Socket, ePacketID PacketID, NetBitStreamInterface& BitStream, SNetExtraInfo* pNetExtraInfo);

private:
    CPlayerManager* m_pPlayerManager;
};
