#ifndef HEADER_OPENHOME_MEDIA_RENDERERS_DUMMY
#define HEADER_OPENHOME_MEDIA_RENDERERS_DUMMY

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Media/Renderer.h>
#include <OpenHome/Private/Timer.h>
#include <iostream>
#include <string>

namespace OpenHome {
namespace Media {

class Dummy : public IRenderer
{
public:
    Dummy();
    ~Dummy();
    virtual void Play(uint32_t aHandle, const ITrack& aTrack, uint32_t aSecond);
    virtual void Pause();
    virtual void Unpause();
    virtual void Stop();
    virtual void FinishAfter(uint32_t aId);
    virtual void SetVolume(uint32_t aValue);
    virtual void SetMute(bool aValue);
    virtual void SetStatusHandler(IRendererStatus& aStatus);
private:
    void Tick();
    void Prefetch();
private:
    Functor iTickFunctor;
    Timer iTickTimer;
    Functor iPrefetchFunctor;
    Timer iPrefetchTimer;
    IRendererStatus* iStatus;

    uint32_t iHandle;
    uint32_t iId;
    std::string iUri;
    uint32_t iSecond;

    uint32_t iDuration;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_RENDERERS_DUMMY

