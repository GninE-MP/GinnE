/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CServerIdManager.h
 *  PURPOSE:
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

class CServerIdManager
{
public:
    virtual ~CServerIdManager() {}
    virtual SString GetConnectionPrivateDirectory(bool bPreviousVer = false) = 0;

    static CServerIdManager* GetSingleton();
};
