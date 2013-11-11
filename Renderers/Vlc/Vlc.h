#ifndef HEADER_OPENHOME_MEDIA_RENDERERS_VLC
#define HEADER_OPENHOME_MEDIA_RENDERERS_VLC

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Media/Renderer.h>
#include <iostream>
#include <string>
#include <vlc/vlc.h>
#include <OpenHome/Private/Timer.h>
#include <OpenHome/Private/Env.h>

namespace OpenHome {
namespace Media {

class Vlc : public IRenderer
{
public:
    Vlc(Environment& aEnv);
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
    void SeekableChanged(const struct libvlc_event_t* aEvent);

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
    uint32_t iSeekToSecond;
    bool iPendingVolume;
    uint32_t iPendingVolumeValue;
    bool iPendingMute;
    bool iPendingMuteValue;
    uint32_t iPendingSeek;
    Functor iTimerFinishedFunctor;
    Timer* iTimerFinished;
    Functor iTimerPlayFunctor;
    Timer* iTimerPlay;
    Mutex iMutex;
    bool iInitialised;
private:
    libvlc_instance_t* iVlc;
    libvlc_media_player_t* iPlayer;
    libvlc_media_t* iMedia;
    IRendererStatus* iStatus;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_RENDERERS_VLC

