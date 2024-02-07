/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTextureDictonarySA.h
 *  PURPOSE:     Header file for game texture dictonary class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <game/RenderWare.h>

class CTextureDictonarySAInterface
{
public:
    RwTexDictionary* rwTexDictonary;
    unsigned short   usUsagesCount;
    unsigned short   usParentIndex;
    unsigned int     hash;
};
