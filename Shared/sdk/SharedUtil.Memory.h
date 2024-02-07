/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Memory.h
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <stddef.h>

namespace SharedUtil
{
    struct ProcessMemoryStats
    {
        size_t virtualMemorySize;
        size_t residentMemorySize;
        size_t sharedMemorySize;
        size_t privateMemorySize;
    };

    bool TryGetProcessMemoryStats(ProcessMemoryStats& out);
}            // namespace SharedUtil
