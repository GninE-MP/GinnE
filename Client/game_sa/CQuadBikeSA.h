/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CQuadBikeSA.h
 *  PURPOSE:     Header file for quad bike vehicle entity class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CQuadBike.h>
#include "CAutomobileSA.h"

class CQuadBikeSAInterface : public CAutomobileSAInterface
{
    // fill this
};

class CQuadBikeSA final : public virtual CQuadBike, public virtual CAutomobileSA
{
public:
    CQuadBikeSA(CQuadBikeSAInterface* pInterface);

    CQuadBikeSAInterface* GetQuadBikeInterface() { return reinterpret_cast<CQuadBikeSAInterface*>(GetInterface()); }
};
