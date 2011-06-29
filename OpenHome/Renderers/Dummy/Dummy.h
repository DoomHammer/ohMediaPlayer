#ifndef HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_DUMMY
#define HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_DUMMY

#include <OhNetTypes.h>
#include "..\..\Renderer.h"
#include <Timer.h>
#include <iostream>
#include <string>

namespace OpenHome {
namespace MediaPlayer {

class Dummy : public IRenderer
{
public:
    Dummy();
    ~Dummy();
    virtual void Play(uint32_t aId, const std::string& aUri, uint32_t aSecond, const std::string& aProvider);
    virtual void Pause();
    virtual void Unpause();
    virtual void Stop();
    virtual void Invalidate(uint32_t aIdFinal);
    virtual void SetStatusHandler(IRendererStatus& aHandler);
private:
    void Tick();
    void Prefetch();
private:
    Functor iTickFunctor;
    Timer iTickTimer;
    Functor iPrefetchFunctor;
    Timer iPrefetchTimer;
    IRendererStatus* iStatus;

    uint32_t iId;
    std::string iUri;
    uint32_t iSecond;
    std::string iProvider;

    uint32_t iDuration;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_DUMMY
