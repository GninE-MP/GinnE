/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTrailerSA.cpp
 *  PURPOSE:     Trailer vehicle entity
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrailerSA.h"

CTrailerSA::CTrailerSA(CTrailerSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
