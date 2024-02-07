/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTrainSA.cpp
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainSA.h"

CTrainSA::CTrainSA(CTrainSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
