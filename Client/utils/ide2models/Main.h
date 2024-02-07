/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/ide2models/Main.h
 *  PURPOSE:     IDE to models converter
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdio>
using namespace std;

int  main(int iArgumentCount, char* szArgs[]);
int  Convert(FILE* pInput, FILE* pOutput);
bool Parse(const char* szLine, FILE* pOutput);
