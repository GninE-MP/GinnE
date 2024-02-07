/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBikeSA.cpp
 *  PURPOSE:     Bike vehicle entity
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBikeSA.h"

CBikeSA::CBikeSA(CBikeSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}

CBikeHandlingEntry* CBikeSA::GetBikeHandlingData()
{
    return m_pBikeHandlingData;
}

void CBikeSA::SetBikeHandlingData(CBikeHandlingEntry* pBikeHandling)
{
    if (!pBikeHandling)
        return;
    m_pBikeHandlingData = static_cast<CBikeHandlingEntrySA*>(pBikeHandling);
    GetBikeInterface()->m_pBikeHandlingData = m_pBikeHandlingData->GetInterface();
    RecalculateBikeHandling();
}

void CBikeSA::RecalculateBikeHandling()
{
    if (m_pBikeHandlingData)
        m_pBikeHandlingData->Recalculate();
}
