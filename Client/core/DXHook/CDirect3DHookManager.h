/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/DXHook/CDirect3DHookManager.h
 *  PURPOSE:     Header file for Direct3D hook manager class
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#pragma once

class CDirect3DHook9;

class CDirect3DHookManager final
{
public:
    ~CDirect3DHookManager();

    void ApplyHook();
    void RemoveHook();

private:
    CDirect3DHook9* m_hook{};
};
