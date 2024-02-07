/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskJumpFall.h
 *  PURPOSE:     Jump/fall task interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"

class CTaskSimpleClimb : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleClimb(){};
};

class CTaskSimpleJetPack : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleJetPack(){};
};
