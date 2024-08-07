/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CObjectRespawner.h
 *  PURPOSE:     Header for object respawner class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CObjectRespawner
{
public:
    CObjectRespawner();
    ~CObjectRespawner() { DoRespawnAll(); };

    void Respawn(CClientObject* pObject);
    void DoRespawnAll();

    bool IsBeingRespawned(CClientObject* pObject);
    void Unreference(CClientObject* pObject);

private:
    std::vector<CClientObject*> m_List;
};
