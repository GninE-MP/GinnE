/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClock.h
 *  PURPOSE:     Game clock class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CClock
{
public:
    CClock();

    void Get(unsigned char& ucHour, unsigned char& ucMinute);
    void Set(unsigned char ucHour, unsigned char ucMinute);

    void          SetMinuteDuration(unsigned long ulDuration);
    unsigned long GetMinuteDuration();

private:
    unsigned long long m_ullMidnightTime;
    unsigned long      m_ulMinuteDuration;
};
