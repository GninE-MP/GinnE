/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerCache.h
 *  PURPOSE:
 *
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

class CServerCacheInterface
{
public:
    virtual ~CServerCacheInterface() {}
    virtual void SaveServerCache(bool bWaitUntilFinished) = 0;
    virtual void GetServerCachedInfo(CServerListItem* pItem) = 0;
    virtual void SetServerCachedInfo(const CServerListItem* pItem) = 0;
    virtual void GetServerListCachedInfo(CServerList* pList) = 0;
    virtual bool GenerateServerList(CServerList* pList) = 0;
};

CServerCacheInterface* GetServerCache();
