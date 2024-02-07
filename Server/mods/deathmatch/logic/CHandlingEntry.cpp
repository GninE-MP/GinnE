/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHandlingEntry.cpp
 *  PURPOSE:     Vehicle handling data entry
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHandlingEntry.h"

CHandlingEntry::CHandlingEntry(tHandlingData* pOriginal)
{
    if (pOriginal)
    {
        // Copy the data from our original
        m_Handling = *pOriginal;
    }
}

// Apply the handling data from another data
void CHandlingEntry::ApplyHandlingData(const CHandlingEntry* pData)
{
    // Copy the data from our handling entry
    m_Handling = pData->m_Handling;
}
