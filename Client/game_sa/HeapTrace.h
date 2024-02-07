/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/HeapTrace.h
 *  PURPOSE:     Heap trace handler
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

void AddTrack(void* pAddress, size_t size, const char* szFile, int iLine);
void RemoveTrack(void* pAddress);
void DumpUnfreed();
