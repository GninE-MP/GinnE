/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedModelInfoSA.cpp
 *  PURPOSE:     Modelinfo for ped entities
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedModelInfoSA.h"

CPedModelInfoSAInterface::CPedModelInfoSAInterface()
{
    MemSetFast(this, 0, sizeof(CPedModelInfoSAInterface));
    VFTBL = (CBaseModelInfo_SA_VTBL*)VAR_CPedModelInfo_VTBL;
    pColModel = (CColModelSAInterface*)VAR_CTempColModels_ModelPed1;
    MemPutFast<DWORD>(&pad, 0xFFFFFFFF);
}

CPedModelInfoSA::CPedModelInfoSA() : CModelInfoSA()
{
    m_pPedModelInterface = new CPedModelInfoSAInterface;
}

void CPedModelInfoSA::SetMotionAnimGroup(AssocGroupId animGroup)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = (DWORD)FUNC_SetMotionAnimGroup;
    _asm
    {
        mov     ecx, dwThis
        push    animGroup
        call    dwFunc
    }
}
