/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CScreenGrabber.h
 *  PURPOSE:
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

typedef void (*PFN_SCREENSHOT_CALLBACK)(const CBuffer* pBuffer, uint uiTimeSpentInQueue, const SString& strError);

//
// CScreenGrabberInterface
//
class CScreenGrabberInterface
{
public:
    ZERO_ON_NEW

    virtual ~CScreenGrabberInterface() {}

    // CScreenGrabberInterface
    virtual void OnDeviceCreate(IDirect3DDevice9* pDevice) = 0;
    virtual void OnLostDevice() = 0;
    virtual void OnResetDevice() = 0;
    virtual void DoPulse() = 0;
    virtual void QueueScreenShot(uint uiSizeX, uint uiSizeY, uint uiQuality, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback) = 0;
    virtual void ClearScreenShotQueue() = 0;
    virtual bool GetBackBufferPixels(uint uiSizeX, uint uiSizeY, CBuffer& buffer, SString& strOutError) = 0;
    virtual bool IsQueueEmpty() = 0;
};

CScreenGrabberInterface* NewScreenGrabber();
