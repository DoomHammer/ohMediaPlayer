#include "Dummy.h"
#include <Debug.h>
#include "..\..\Player.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;
using namespace std;

Dummy::Dummy()
    : iTickFunctor(MakeFunctor(*this, &Dummy::Tick))
    , iTickTimer(iTickFunctor)
    , iPrefetchFunctor(MakeFunctor(*this, &Dummy::Prefetch))
    , iPrefetchTimer(iPrefetchFunctor)
{
}

Dummy::~Dummy()
{
}

void Dummy::Play(uint32_t aHandle, const class ITrack* aTrack, uint32_t aSecond)
{
    iHandle = aHandle;
    iId = aTrack->Id();
    const uint8_t* uri;
    uint32_t bytes;
    aTrack->Uri(uri, bytes);
    iUri.assign((const char*)uri, bytes);
    iSecond = aSecond;

    cout << "Dummy::Play called with id: " << iId << " Uri: " << iUri << endl;

    iDuration = 10; //10 seconds

    iPrefetchTimer.FireIn(500);
}

void Dummy::Pause()
{
    cout << "Dummy::Pause called" << endl;
    iTickTimer.Cancel();
    iStatus->Paused(iHandle, iId);
}

void Dummy::Unpause()
{
    cout << "Dummy::Unpause called" << endl;
    iTickTimer.FireIn(1000);
}

void Dummy::Stop()
{
    cout << "Dummy::Stop called" << endl;
    iTickTimer.Cancel();
    iStatus->Stopped(iHandle, iId);
}

void Dummy::Invalidate(uint32_t aIdFinal)
{
    cout << "Dummy::Invalidate(" << aIdFinal << ")" << endl;
}

void Dummy::SetStatusHandler(IRendererStatus& aHandler) 
{
    iStatus = &aHandler;
}

void Dummy::Prefetch()
{
    iStatus->Started(iHandle, iId, iDuration, 128000, 24, 44100, false, "mp3");
    iStatus->Playing(iHandle, iId, iSecond);
    iTickTimer.FireIn(1000);
}

void Dummy::Tick()
{
    ++iSecond;
    iStatus->Playing(iHandle, iId, iSecond);
    if(iSecond == iDuration) {
        iStatus->Finished(iHandle, iId); 
    }
    else {
        iTickTimer.FireIn(1000);
    }
}
