/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  GninE is available from https://www.gnine.com/
 *
 *****************************************************************************/

class CClientGifDisplay;

#pragma once

#include "CClientDisplay.h"
#include "CClientDisplayManager.h"

class CClientGifDisplay final : public CClientDisplay
{
    friend class CClientDisplayManager;

public:
    CClientGifDisplay(CClientDisplayManager* pDisplayManager, CClientGif* pGif, int ID = DISPLAY_GIF);
    ~CClientGifDisplay() = default;
    eDisplayType GetType() { return DISPLAY_GIF; }
    void         Render();
    void         UpdateTexture();
    void         ClearTexture();
    bool         IsCleared() { return m_bIsCleared; }

private:
    CClientGif* m_pGif;
    bool        m_bIsCleared;
};
