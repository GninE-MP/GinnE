/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeapon.h
 *  PURPOSE:     Weapon entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CWeaponInfo.h"

class CColPoint;
class CPed;
class CVector;
class CVector2D;
class CWeaponInfo;
class CWeaponStat;
enum ePedPieceTypes;
struct SLineOfSightBuildingResult;
struct SLineOfSightFlags;
struct SWeaponConfiguration;

class CWeapon
{
public:
    virtual eWeaponType  GetType() = 0;
    virtual void         SetType(eWeaponType type) = 0;
    virtual eWeaponState GetState() = 0;
    virtual void         SetState(eWeaponState state) = 0;
    virtual DWORD        GetAmmoInClip() = 0;
    virtual void         SetAmmoInClip(DWORD dwAmmoInClip) = 0;
    virtual DWORD        GetAmmoTotal() = 0;
    virtual void         SetAmmoTotal(DWORD dwAmmoTotal) = 0;
    virtual CPed*        GetPed() = 0;
    virtual eWeaponSlot  GetSlot() = 0;
    virtual void         SetAsCurrentWeapon() = 0;
    virtual CWeaponInfo* GetInfo(eWeaponSkill skill) = 0;
    virtual void         Remove() = 0;
    virtual void         Destroy() = 0;
    virtual void         Initialize(eWeaponType type, unsigned int uiAmmo, CPed* pPed) = 0;
    virtual void         Update(CPed* pPed) = 0;
    virtual bool         Fire(CEntity* pFiringEntity, CVector* pvecOrigin, CVector* pvecOffset, CEntity* pTargetEntity, CVector* pvec_1, CVector* pvec2) = 0;
    virtual void         AddGunshell(CEntity* pFiringEntity, CVector* pvecOrigin, CVector2D* pvecDirection, float fSize) = 0;
    virtual void DoBulletImpact(CEntity* pFiringEntity, CEntitySAInterface* pEntityInterface, CVector* pvecOrigin, CVector* pvecTarget, CColPoint* pColPoint,
                                int i_1) = 0;
    virtual unsigned char GenerateDamageEvent(CPed* pPed, CEntity* pResponsible, eWeaponType weaponType, int iDamagePerHit, ePedPieceTypes hitZone,
                                              int i_2) = 0;
    virtual bool          ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity,
                                             const SLineOfSightFlags flags, SLineOfSightBuildingResult* pBuildingResult, eWeaponType weaponType,
                                             CEntitySAInterface** pEntity) = 0;
    virtual bool FireInstantHit(CEntity* pFiringEntity, const CVector* pvecOrigin, const CVector* pvecMuzzle, CEntity* pTargetEntity, const CVector* pvecTarget,
                                const CVector* pvec, bool bCrossHairGun, bool bCreateGunFx) = 0;
    virtual bool FireBullet(CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget) = 0;
    virtual int  GetWeaponReloadTime(CWeaponStat* pWeaponStat) = 0;
};
