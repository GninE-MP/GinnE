/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/Platform.cpp
 *  PURPOSE:     Platform-specific defines and methods
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

extern "C" bool g_bSilent;
extern "C" bool g_bNoTopBar;

/** Operating system identifiers **/
#if defined(WIN32)
    #ifdef _WIN64
        #define GninE_OS_STRING   "Windows x64"
    #elif defined(_M_ARM64)
        #define GninE_OS_STRING   "Windows arm64"
    #elif defined(_M_ARM)
        #define GninE_OS_STRING   "Windows arm"
    #else
        #define GninE_OS_STRING   "Windows"
    #endif
    #define GninE_LIB_EXTENSION   ".dll"
#elif defined(__linux__)
    #ifdef __x86_64__
        #define GninE_OS_STRING   "GNU/Linux x64"
    #elif defined(__aarch64__)
        #define GninE_OS_STRING   "GNU/Linux arm64"
    #elif defined(__arm__)
        #define GninE_OS_STRING   "GNU/Linux arm"
    #else
        #define GninE_OS_STRING   "GNU/Linux"
    #endif
    #define GninE_LIB_EXTENSION   ".so"
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #define GninE_OS_STRING       "BSD"
    #define GninE_LIB_EXTENSION   ".so"
#elif defined(__APPLE__)
    #define GninE_OS_STRING       "macOS"
    #define GninE_LIB_EXTENSION   ".dylib"
#else
    #error "Unsupported operating system"
#endif

#if defined(GninE_DEBUG)
    #define GninE_LIB_SUFFIX  "_d"
#else
    #define GninE_LIB_SUFFIX
#endif

#ifdef WIN32
// Define types
typedef int socklen_t;

// Define keys
    #define KEY_BACKSPACE 0x08
    #define KEY_EXTENDED  0xE0
    #define KEY_LEFT      0x4B
    #define KEY_RIGHT     0x4D
    #define KEY_UP        0x48
    #define KEY_DOWN      0x50
#endif
