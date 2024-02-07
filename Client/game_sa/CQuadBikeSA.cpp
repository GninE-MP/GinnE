/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CQuadBikeSA.cpp
 *  PURPOSE:     Quad bike vehicle entity
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CQuadBikeSA.h"

CQuadBikeSA::CQuadBikeSA(CQuadBikeSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
