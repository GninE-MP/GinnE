/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CColModel.h
 *  PURPOSE:     Collision model entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

struct CColModelSAInterface;

class CColModel
{
protected:
    ~CColModel() = default;

public:
    virtual CColModelSAInterface* GetInterface() = 0;
    virtual void                  Destroy() = 0;
};
