/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/XML.cpp
 *  PURPOSE:     XML module entry
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CXMLImpl* pXMLInterface = NULL;

GninEEXPORT CXML* InitXMLInterface(const char* szSaveFlagDirectory)
{
    // Initialize and do any file recovery as necessary
    CXMLFileImpl::InitFileRecovery(szSaveFlagDirectory);

    // this is required during parsing, see resources project fallout/help.xml @r659
    TiXmlBase::SetCondenseWhiteSpace(false);

    pXMLInterface = new CXMLImpl;
    return pXMLInterface;
}

GninEEXPORT void ReleaseXMLInterface()
{
    if (pXMLInterface)
    {
        delete pXMLInterface;
        pXMLInterface = NULL;
    }
}

#ifdef WIN32

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    // Kill the interface on detach
    if (dwReason == DLL_PROCESS_DETACH)
    {
        ReleaseXMLInterface();
    }

    return TRUE;
}

#else

void __attribute__((destructor)) ReleaseXMLInterface(void);

#endif
