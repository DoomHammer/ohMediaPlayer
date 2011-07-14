#ifndef HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_VLC
#define HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_VLC

#include <OhNetTypes.h>
#include "..\..\Renderer.h"
#include <iostream>
#include <string>
#include <vlc/vlc.h>
#include <Timer.h>

namespace OpenHome {
namespace MediaPlayer {

class Vlc : public IRenderer
{
public:
    Vlc();
    ~Vlc();
    virtual void Play(uint32_t aHandle, const ITrack& aTrack, uint32_t aSecond);
    virtual void Pause();
    virtual void Unpause();
    virtual void Stop();
    virtual void FinishAfter(uint32_t aId);
    virtual void SetVolume(uint32_t aValue);
    virtual void SetMute(bool aValue);
    virtual void SetStatusHandler(IRendererStatus& aStatus);

public:
    void DurationChanged(const struct libvlc_event_t* aEvent);
    void ParsedChanged(const struct libvlc_event_t* aEvent);
    void StateChanged(const struct libvlc_event_t* aEvent);
    void Playing(const struct libvlc_event_t* aEvent);
    void EndReached(const struct libvlc_event_t* aEvent);
    void EncounteredError(const struct libvlc_event_t* aEvent);
    void TimeChanged(const struct libvlc_event_t* aEvent);

private:
    void TimerFinishedExpired();
    void TimerPlayExpired();
    void CleanupLocked();
    
private:
    uint32_t iHandle;
    uint32_t iId;
    std::string iUri;
    uint32_t iSeconds;
    uint32_t iDuration;
    Functor iTimerFinishedFunctor;
    Timer* iTimerFinished;
    Functor iTimerPlayFunctor;
    Timer* iTimerPlay;
    Mutex iMutex;
    bool iInitialised;
private:
    libvlc_instance_t* iVlc;
    libvlc_log_t* iVlcLog;
    libvlc_media_player_t* iPlayer;
    libvlc_media_t* iMedia;
    IRendererStatus* iStatus;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_RENDERERS_VLC

