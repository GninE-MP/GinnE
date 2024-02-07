/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

class CClientModelCacheManager
{
public:
    virtual ~CClientModelCacheManager() {}

    // CClientModelCacheManager interface
    virtual void DoPulse() = 0;
    virtual void OnRestreamModel(ushort usModelId) = 0;
    virtual void SetCustomLimits(std::optional<size_t> numVehicles, std::optional<size_t> numPeds) = 0;
};

CClientModelCacheManager* NewClientModelCacheManager();
