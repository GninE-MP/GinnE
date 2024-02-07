//
// Shared/version.h
//
#pragma once

/*******************************************************************************
 * Note for fork developers:
 *
 * If you don't follow these guidelines, you may suffer from inadequate or even
 * zero anti-cheat support, when you release a fork client to the public with a
 * misconfigured deployment pipeline. Never use build type VERSION_TYPE_CUSTOM in
 * a public release of your fork.
 *
 * Please read the wiki article at https://wiki.gnine.com/wiki/Forks for
 * more information, guidelines and security considerations.
 *
 * To compile a client for mass consumption by your player base (as a public release):
 *   1. Set GninESA_VERSION_TYPE to VERSION_TYPE_UNSTABLE
 *   2. Use netc.dll from https://mirror-cdn.gnine.com/bdata/fork-support/netc.dll
 *   3. Switch to Nightly build configuration
 *
 * To develop and test a client locally or in a trustworthy closed group (local or private release):
 *  a. With active anti-cheat:
 *      1. Set GninESA_VERSION_TYPE to VERSION_TYPE_UNSTABLE
 *      2. Use netc.dll from https://mirror-cdn.gnine.com/bdata/netc.dll (preferably run utils/buildactions/install_data.lua)
 *  b. Without anti-cheat:
 *      1. Set GninESA_VERSION_TYPE to VERSION_TYPE_CUSTOM
 *      2. Use netc.dll from https://mirror-cdn.gnine.com/bdata/netc.dll (preferably run utils/buildactions/install_data.lua)
 *
 * To compile and host an official public server (anyone on official GninE client can connect):
 *      1. Copy net.dll or net.so from a nightly release (see https://nightly.gnine.com/)
 *      2. Set GninESA_VERSION_TYPE to VERSION_TYPE_RELEASE
 *      3. Set GninESA_VERSION_BUILD to the build number from the network library (see file version information)
 *      4. Switch to Nightly build configuration
 *
 * To compile and host a custom public server (only custom GninE clients can connect):
 *      1. Use `<disableac>5,6,21</disableac>` in your GninEserver.conf (may not be needed in the future)
 *      2. Use regular developer net.dll or net.so (preferably run utils/buildactions/install_data.lua)
 *      3. Set GninESA_VERSION_TYPE to VERSION_TYPE_UNSTABLE
 *      4. Switch to Nightly build configuration
 *
 *******************************************************************************/

// New version info
#define GninESA_VERSION_MAJOR         1
#define GninESA_VERSION_MINOR         6
#define GninESA_VERSION_MAINTENANCE   0
#define GninESA_VERSION_TYPE          VERSION_TYPE_CUSTOM
#define GninESA_VERSION_BUILD         0

#include "../build_overrides.h"

// Old version info
#define GninE_DM_VERSION              ( ( ( GninESA_VERSION_MAJOR ) << 8 ) | ( ( GninESA_VERSION_MINOR ) << 4 ) | ( ( GninESA_VERSION_MAINTENANCE ) << 0 ) )
#if GninESA_VERSION_MAINTENANCE == 0
    #define GninE_DM_VERSIONSTRING        QUOTE_DEFINE ( GninESA_VERSION_MAJOR ) "." QUOTE_DEFINE ( GninESA_VERSION_MINOR )
#else
    #define GninE_DM_VERSIONSTRING        QUOTE_DEFINE ( GninESA_VERSION_MAJOR ) "." QUOTE_DEFINE ( GninESA_VERSION_MINOR ) "." QUOTE_DEFINE ( GninESA_VERSION_MAINTENANCE )
#endif

#ifdef GninE_CLIENT
#define GninE_DM_FULL_STRING "GninE:SA Client"
#else
#define GninE_DM_FULL_STRING "GninE:SA Server"
#endif

// Compile types
#define VERSION_TYPE_CUSTOM         0x01
#define VERSION_TYPE_EXPERIMENTAL   0x03
#define VERSION_TYPE_UNSTABLE       0x05
#define VERSION_TYPE_UNTESTED       0x07
#define VERSION_TYPE_RELEASE        0x09

#define QUOTE_DEFINE2(c) #c
#define QUOTE_DEFINE(x) QUOTE_DEFINE2(x)

// Implement compile types
#if GninESA_VERSION_TYPE == VERSION_TYPE_CUSTOM

    #define GninE_DM_BUILDTYPE        "custom"
    #define GninE_DM_BUILDTAG_SHORT   GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE
    #define GninE_DM_BUILDTAG_LONG    GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE

#elif GninESA_VERSION_TYPE == VERSION_TYPE_UNSTABLE

    #define GninE_DM_BUILDTYPE        "unstable"
    #define GninE_DM_BUILDTAG_SHORT   GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE "-" QUOTE_DEFINE ( GninESA_VERSION_BUILD )
    #define GninE_DM_BUILDTAG_LONG    GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE "-" QUOTE_DEFINE ( GninESA_VERSION_BUILD )

#elif GninESA_VERSION_TYPE == VERSION_TYPE_UNTESTED

    #define GninE_DM_BUILDTYPE        "untested"
    #define GninE_DM_BUILDTAG_SHORT   GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE "-" QUOTE_DEFINE ( GninESA_VERSION_BUILD )
    #define GninE_DM_BUILDTAG_LONG    GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE "-" QUOTE_DEFINE ( GninESA_VERSION_BUILD )
    #define GninE_DM_PUBLIC_CONNECTIONS
    #define GninE_DM_EXPIRE_DAYS      60

#elif GninESA_VERSION_TYPE == VERSION_TYPE_RELEASE

    #define GninE_DM_BUILDTYPE        "release"
    #define GninE_DM_BUILDTAG_SHORT   GninE_DM_VERSIONSTRING
    #define GninE_DM_BUILDTAG_LONG    GninE_DM_VERSIONSTRING "-" GninE_DM_BUILDTYPE "-" QUOTE_DEFINE ( GninESA_VERSION_BUILD )
    #define GninE_DM_PUBLIC_CONNECTIONS

#else
    #error "Incorrect GninESA_VERSION_TYPE"
#endif

#define _ASE_VERSION QUOTE_DEFINE(GninESA_VERSION_MAJOR) "." QUOTE_DEFINE(GninESA_VERSION_MINOR)
#define _NETCODE_VERSION_BRANCH_ID      0x4         // Use 0x1 - 0xF to indicate an incompatible branch is being used (0x0 is reserved, 0x4 is trunk)
#define _CLIENT_NET_MODULE_VERSION      0x0AD       // (0x000 - 0xfff) Lvl9 wizards only
#define _SERVER_NET_MODULE_VERSION      0x0AB       // (0x000 - 0xfff) Lvl9 wizards only
#define _NETCODE_VERSION                0x1DA       // (0x000 - 0xfff) Increment when net messages change (pre-release)

// (0x000 - 0xfff) Update bitstream.h when net messages change (post-release). (Changing will also require additional backward compatibility code).
#define GninE_DM_BITSTREAM_VERSION eBitStreamVersion::Latest

// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(GninE_DM_PUBLIC_CONNECTIONS)
    #define GninE_DM_ASE_VERSION                  _ASE_VERSION
    #define GninE_DM_NETCODE_VERSION              _NETCODE_VERSION
    #define GninE_DM_CLIENT_NET_MODULE_VERSION    _CLIENT_NET_MODULE_VERSION
    #define GninE_DM_SERVER_NET_MODULE_VERSION    _SERVER_NET_MODULE_VERSION
#else
    #if _NETCODE_VERSION_BRANCH_ID < 1 || _NETCODE_VERSION_BRANCH_ID > 15
        #error "_NETCODE_VERSION_BRANCH_ID wrong"
    #endif
    #define GninE_DM_ASE_VERSION                  _ASE_VERSION "n"
    #define GninE_DM_NETCODE_VERSION              ( _NETCODE_VERSION + ( _NETCODE_VERSION_BRANCH_ID << 12 ) )
    #define GninE_DM_CLIENT_NET_MODULE_VERSION    ( _CLIENT_NET_MODULE_VERSION + ( 4 << 12 ) )
    #define GninE_DM_SERVER_NET_MODULE_VERSION    _SERVER_NET_MODULE_VERSION
#endif

// Handy self compile message
#ifndef GninE_DM_PUBLIC_CONNECTIONS
    #if defined(SHOW_SELF_COMPILE_WARNING) && !defined(CI_BUILD) && !defined(GninE_CLIENT)
        #pragma message("\n\
----------------------------------------------------------------------\n\
GninESA_VERSION_TYPE is not set to VERSION_TYPE_RELEASE\n\
Server will not work with release clients\n\
-------------------------------------------------------------------------\n\
If you want the server to work with release clients\n\
set GninESA_VERSION_TYPE to VERSION_TYPE_RELEASE in Server/version.h\n\
\n\
Client can't enter the game? Use Server net.dll from an official build.\n\
(Official dll uses VERSION_TYPE_RELEASE, win-install-data gives UNSTABLE.)\n\
-------------------------------------------------------------------------")
    #endif
#endif
