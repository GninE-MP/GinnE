/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIMessageBox.h
 *  PURPOSE:     Message box interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUIWindow.h"
#include "CGUICallback.h"

class CGUIMessageBox
{
public:
    virtual ~CGUIMessageBox(){};

    virtual void SetAutoDestroy(bool bAutoDestroy) = 0;
    virtual bool GetAutoDestroy() = 0;

    virtual void SetVisible(bool bVisible) = 0;
    virtual bool IsVisible() = 0;

    virtual void SetClickHandler(GUI_CALLBACK Callback) = 0;

    virtual CGUIWindow* GetWindow() = 0;
};
