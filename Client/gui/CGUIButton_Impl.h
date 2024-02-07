/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIButton_Impl.h
 *  PURPOSE:     Button widget class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIButton.h>
#include "CGUIElement_Impl.h"

class CGUIButton_Impl : public CGUIButton, public CGUIElement_Impl
{
public:
    CGUIButton_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "");
    ~CGUIButton_Impl();

    eCGUIType GetType() { return CGUI_BUTTON; };

    #include "CGUIElement_Inc.h"
};
