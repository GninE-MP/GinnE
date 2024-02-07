/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.h
 *  PURPOSE:     Header file for projectile entity class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileSAInterface : public CObjectSAInterface            // entirely inherited from CObject
{
};

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
private:
    CProjectileSAInterface* internalInterface;
    class CProjectileInfo*  projectileInfo;
    bool                    m_bDestroyed;

public:
    CProjectileSA(class CProjectileSAInterface* projectileInterface);
    ~CProjectileSA();
    void Destroy(bool bBlow = true);
    bool CalculateImpactPosition(CEntitySAInterface* pCollidedWith, CVector vecInputStart, CVector& vecInputEnd);

    void SetProjectileInfo(CProjectileInfo* pProjectileInfo) { projectileInfo = pProjectileInfo; }
    bool CorrectPhysics();
};
