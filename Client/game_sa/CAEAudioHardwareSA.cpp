/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEAudioHardwareSA.cpp
 *  PURPOSE:     Audio hardware
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAEAudioHardwareSA.h"

CAEAudioHardwareSA::CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface)
{
    m_pInterface = pInterface;
}

bool CAEAudioHardwareSA::IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID)
{
    DWORD dwSoundBankID = wSoundBankID;
    DWORD dwSoundBankSlotID = wSoundBankSlotID;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAEAudioHardware__IsSoundBankLoaded;
    bool  bReturn = false;
    _asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CAEAudioHardwareSA::LoadSoundBank(short wSoundBankID, short wSoundBankSlotID)
{
    DWORD dwSoundBankID = wSoundBankID;
    DWORD dwSoundBankSlotID = wSoundBankSlotID;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAEAudioHardware__LoadSoundBank;
    _asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
    }
}
