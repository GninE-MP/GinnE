/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CIFPAnimations.h
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientIFP.h"

class CIFPAnimations
{
public:
    struct SAnimation
    {
        SString                              Name;
        unsigned int                         uiNameHash;
        std::unique_ptr<CAnimBlendHierarchy> pHierarchy;
        BYTE*                                pSequencesMemory;
    };

    std::vector<SAnimation> vecAnimations;

public:
    ~CIFPAnimations();
    void DeleteAnimations();
};
