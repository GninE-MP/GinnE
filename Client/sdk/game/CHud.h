/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHud.h
 *  PURPOSE:     HUD interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

enum eHudComponent
{
    // Order must be preserved for net comms
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
    HUD_VITAL_STATS,
    HUD_HELP_TEXT,
};

class CHud
{
public:
    virtual void Disable(bool bDisabled) = 0;
    virtual bool IsDisabled() = 0;
    virtual void SetComponentVisible(eHudComponent component, bool bVisible) = 0;
    virtual bool IsComponentVisible(eHudComponent component) = 0;
    virtual void AdjustComponents(float fAspectRatio) = 0;
    virtual void ResetComponentAdjustment() = 0;
};
