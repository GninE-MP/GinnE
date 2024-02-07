/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehiclPushPacket.cpp
 *  PURPOSE:     Unoccupied vehicle push synchronization packet class ( Change syncer to the pusher of the vehicle to make it neater. )
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CUnoccupiedVehiclePushPacket.h"

bool CUnoccupiedVehiclePushPacket::Read(NetBitStreamInterface& BitStream)
{
    if (BitStream.Read(&vehicle))
    {
        return true;
    }
    return false;
}
