/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.h
 *  PURPOSE:     Header for element group class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

class CElementGroup;

#pragma once

#include "CClientEntity.h"

class CElementGroup
{
private:
    CFastList<CClientEntity*> m_elements;

public:
    ~CElementGroup();
    void         Add(CClientEntity* element);
    void         Remove(CClientEntity* element);
    unsigned int GetCount();
};
