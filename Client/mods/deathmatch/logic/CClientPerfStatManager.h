/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientPerfStatManager.h
 *  PURPOSE:     Performance stats manager class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "CClientPerfStatModule.h"

//
// CClientPerfStatManager
//
class CClientPerfStatManager
{
public:
    virtual ~CClientPerfStatManager() {}

    virtual void DoPulse() = 0;
    virtual void GetStats(CClientPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter) = 0;

    static CClientPerfStatManager* GetSingleton();
};
