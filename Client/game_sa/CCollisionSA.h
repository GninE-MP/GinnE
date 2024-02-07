/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCollisionSA.h
 *  PURPOSE:     Header file for `CCollision` - collision detection related functions
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "game/CCollision.h"

class CCollisionSA : CCollision {
public:
    bool TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) const override;
};
