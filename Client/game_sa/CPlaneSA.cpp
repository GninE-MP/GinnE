/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.cpp
 *  PURPOSE:     Plane vehicle entity
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlaneSA.h"

CPlaneSA::CPlaneSA(CPlaneSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
