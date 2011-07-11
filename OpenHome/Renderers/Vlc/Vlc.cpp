#include "Vlc.h"
#include <Debug.h>
#include "..\..\Player.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;
using namespace std;

void DurationChanged(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->DurationChanged(aEvent);
}

void ParsedChanged(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->ParsedChanged(aEvent);
}

void StateChanged(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->StateChanged(aEvent);
}

void Playing(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->Playing(aEvent);
}

void EndReached(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->EndReached(aEvent);
}

void EncounteredError(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->EncounteredError(aEvent);
}

void TimeChanged(const struct libvlc_event_t* aEvent, void* aContext)
{
    ((Vlc*)aContext)->TimeChanged(aEvent);
}

Vlc::Vlc()
    : iVlc(0)
    , iVlcLog(0)
    , iPlayer(0)
    , iMedia(0)
    , iStatus(0)
{
    iVlc = libvlc_new (0, NULL);
    libvlc_set_log_verbosity(iVlc, 3);
    iVlcLog = libvlc_log_open(iVlc);
    iTimerFinishedFunctor = MakeFunctor(*this, &Vlc::TimerFinishedExpired);
    iTimerFinished = new Timer(iTimerFinishedFunctor);
}

Vlc::~Vlc()
{
    libvlc_log_close(iVlcLog);
    libvlc_release(iVlc);
    delete iTimerFinished;
}

void Vlc::Play(uint32_t aHandle, const class ITrack& aTrack, uint32_t aSecond)
{
    if(iMedia) {
        libvlc_event_manager_t* mediaEvent = libvlc_media_event_manager(iMedia);
        libvlc_event_detach(mediaEvent, libvlc_MediaDurationChanged, ::DurationChanged, this);
        libvlc_event_detach(mediaEvent, libvlc_MediaParsedChanged, ::ParsedChanged, this);
        libvlc_event_detach(mediaEvent, libvlc_MediaStateChanged, ::StateChanged, this);
        libvlc_media_release(iMedia);
        iMedia = 0;
    }
    if(iPlayer) {
        libvlc_event_manager_t* playerEvent = libvlc_media_player_event_manager(iPlayer);
        libvlc_event_detach(playerEvent, libvlc_MediaPlayerPlaying, ::Playing, this);
        libvlc_event_detach(playerEvent, libvlc_MediaPlayerEndReached, ::EndReached, this);
        libvlc_event_detach(playerEvent, libvlc_MediaPlayerEncounteredError, ::EncounteredError, this);
        libvlc_event_detach(playerEvent, libvlc_MediaPlayerTimeChanged, ::TimeChanged, this);
        libvlc_media_player_stop(iPlayer);
        libvlc_media_player_release(iPlayer); 
        iPlayer = 0;
    }

    const uint8_t* uri;
    uint32_t bytes;
    aTrack.Uri(uri, bytes);
    iUri.assign((const char*)uri, bytes);
    iHandle = aHandle;
    iId = aTrack.Id();
    iSeconds = aSecond;
    iDuration = -1;

    iMedia = libvlc_media_new_location(iVlc, iUri.c_str());
    iPlayer = libvlc_media_player_new_from_media(iMedia);

    libvlc_event_manager_t* mediaEvent = libvlc_media_event_manager(iMedia);
    libvlc_event_manager_t* playerEvent = libvlc_media_player_event_manager(iPlayer);

    int32_t ret;
    ret = libvlc_event_attach(mediaEvent, libvlc_MediaDurationChanged, ::DurationChanged, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(mediaEvent, libvlc_MediaParsedChanged, ::ParsedChanged, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(mediaEvent, libvlc_MediaStateChanged, ::StateChanged, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(playerEvent, libvlc_MediaPlayerPlaying, ::Playing, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(playerEvent, libvlc_MediaPlayerEndReached, ::EndReached, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(playerEvent, libvlc_MediaPlayerEncounteredError, ::EncounteredError, this);
    ASSERT(ret == 0);
    ret = libvlc_event_attach(playerEvent, libvlc_MediaPlayerTimeChanged, ::TimeChanged, this);
    ASSERT(ret == 0);

    libvlc_media_player_play(iPlayer);
}

void Vlc::Pause()
{
    libvlc_media_player_pause(iPlayer);
}

void Vlc::Unpause()
{
    libvlc_media_player_pause(iPlayer);
}

void Vlc::Stop()
{
    libvlc_media_player_stop(iPlayer);
}

void Vlc::FinishAfter(uint32_t aId)
{
    cout << "Vlc::FinishAfter(" << aId << ")" << endl;
}

void Vlc::SetStatusHandler(IRendererStatus& aHandler) 
{
    iStatus = &aHandler;
}

void Vlc::DurationChanged(const struct libvlc_event_t* aEvent)
{
    iDuration = (uint32_t)(aEvent->u.media_duration_changed.new_duration/1000);
}

void Vlc::ParsedChanged(const struct libvlc_event_t* aEvent)
{
    Log::Print("Vlc::ParsedChanged(%d)\n", aEvent->u.media_parsed_changed.new_status);
}

void Vlc::StateChanged(const struct libvlc_event_t* aEvent)
{
    Log::Print("Vlc::StateChanged(%d)\n", aEvent->u.media_state_changed.new_state);
}

void Vlc::Playing(const struct libvlc_event_t* aEvent)
{
    Log::Print("Vlc::Playing\n");
    iStatus->Playing(iHandle, iId, iDuration, 128000, 24, 44100, false, "mp3");
}

void Vlc::EndReached(const struct libvlc_event_t* aEvent)
{
    Log::Print("Vlc::EndReached\n");
    //Finished calls Vlc::Play, which calls back into vlc.  This isn't allowed.
    //Call Finished from another thrad.
    iTimerFinished->FireIn(0);
}

void Vlc::EncounteredError(const struct libvlc_event_t* aEvent)
{
    Log::Print("Vlc::EncounteredError\n");
    //Finished calls Vlc::Play, which calls back into vlc.  This isn't allowed.
    //Call Finished from another thrad.
    iTimerFinished->FireIn(0); 
}

void Vlc::TimeChanged(const struct libvlc_event_t* aEvent)
{
    uint64_t ms = aEvent->u.media_player_time_changed.new_time;
    uint32_t sec = ms / 1000;

    if(sec > iSeconds) {
        iSeconds = sec;
        iStatus->Time(iHandle, iId, iSeconds);
    }
}

void Vlc::TimerFinishedExpired()
{
    Log::Print("Vlc::TimerFinishedExpired\n");
    iStatus->Finished(iHandle, iId);
}

