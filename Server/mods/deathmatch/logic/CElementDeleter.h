/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementDeleter.h
 *  PURPOSE:     Element lazy deletion manager class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"
#include <list>

class CElementDeleter
{
public:
    ~CElementDeleter() { DoDeleteAll(); };

    void Delete(class CElement* pElement, bool bUnlink = true, bool bUpdatePerPlayerEntities = true);
    void DoDeleteAll();

    bool IsBeingDeleted(class CElement* pElement);
    void Unreference(CElement* pElement);

    void CleanUpForVM(CLuaMain* pLuaMain);

private:
    CElementListType m_List;
};
