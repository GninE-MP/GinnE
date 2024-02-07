/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.h
 *  PURPOSE:     Header for resource file
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CDownloadableResource.h"

class CResourceFile : public CDownloadableResource
{
public:
    CResourceFile(CResource* pResource, eResourceType resourceType, const char* szShortName, const char* szResourceFileName, uint uiDownloadSize,
                  CChecksum serverChecksum, bool bAutoDownload)
        : CDownloadableResource(pResource, resourceType, szResourceFileName, szShortName, uiDownloadSize, serverChecksum, bAutoDownload)
    {
    }
};
