/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAESoundManager.h
 *  PURPOSE:     Game audio engine manager interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/
#pragma once

class CAESoundManager
{
public:
    virtual void CancelSoundsInBankSlot(uint uiGroup, uint uiIndex) = 0;
};
