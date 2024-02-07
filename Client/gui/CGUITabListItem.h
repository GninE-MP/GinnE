/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabListItem.h
 *  PURPOSE:     Tab-able Element Interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/
#pragma once

#include <gui/CGUI.h>

class CGUITabListItem : public CGUIElement
{
public:
    virtual bool ActivateOnTab() = 0;
};
