/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBmxSA.h
 *  PURPOSE:     Header file for bmx bike vehicle entity class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBmx.h>
#include "CBikeSA.h"

class CBmxSAInterface : public CBikeSAInterface
{
    // fill this
};

class CBmxSA final : public virtual CBmx, public virtual CBikeSA
{
public:
    CBmxSA(CBmxSAInterface* pInterface);

    CBmxSAInterface* GetBmxInterface() { return reinterpret_cast<CBmxSAInterface*>(GetInterface()); }
};
