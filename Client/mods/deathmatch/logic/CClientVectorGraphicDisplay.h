/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  GninE is available from https://www.gnine.com/
 *
 *****************************************************************************/

class CClientVectorGraphicDisplay;

#pragma once

#include "CClientDisplay.h"
#include "CClientDisplayManager.h"

class CClientVectorGraphicDisplay final : public CClientDisplay
{
    friend class CClientDisplayManager;

public:
    CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID = DISPLAY_VECTORGRAPHIC);
    ~CClientVectorGraphicDisplay() = default;

    eDisplayType GetType() { return DISPLAY_VECTORGRAPHIC; }

    void Render();

    void UpdateTexture();
    void ClearTexture();

    bool IsCleared() const { return m_bIsCleared; }
    bool HasUpdated() const { return m_bHasUpdated; }

    void Update();

private:
    CClientVectorGraphic* m_pVectorGraphic;

    bool m_bIsCleared;
    bool m_bHasUpdated;
};
