/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHeliSA.h
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHeli.h>
#include "CAutomobileSA.h"

class CHeliSAInterface : public CAutomobileSAInterface
{
};

class CHeliSA final : public virtual CHeli, public virtual CAutomobileSA
{
public:
    CHeliSA(CHeliSAInterface* pInterface);
};
