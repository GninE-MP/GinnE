/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIProgressBar.h
 *  PURPOSE:     Progress bar widget interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include <string>

class CGUIProgressBar : public CGUIElement
{
public:
    virtual ~CGUIProgressBar(){};

    virtual void  SetProgress(float fProgress) = 0;
    virtual float GetProgress() = 0;
};
