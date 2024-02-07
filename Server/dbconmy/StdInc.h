/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        StdInc.h
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>

#define SHARED_UTIL_WITH_FAST_HASH_MAP
#include <SharedUtil.h>
#include <GninEPlatform.h>

#ifdef WIN32
//
// Windows
//
    #include <windows.h>
    #include <mmsystem.h>
#endif
