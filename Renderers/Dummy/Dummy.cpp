#include "Dummy.h"
#include <OpenHome/Private/Debug.h>
#include <OpenHome/Media/Player.h>

using namespace OpenHome;
using namespace OpenHome::Media;
using namespace std;

Dummy::Dummy(Environment& aEnv)
    : iTickFunctor(MakeFunctor(*this, &Dummy::Tick))
    , iTickTimer(aEnv, iTickFunctor, "DummyRendererTick")
    , iPrefetchFunctor(MakeFunctor(*this, &Dummy::Prefetch))
    , iPrefetchTimer(aEnv, iPrefetchFunctor, "DummyRendererPrefetch")
{
}

Dummy::~Dummy()
{
}

void Dummy::Play(uint32_t aHandle, const class ITrack& aTrack, uint32_t aSecond)
{
    iHandle = aHandle;
    iId = aTrack.Id();
    const uint8_t* uri;
    uint32_t bytes;
    aTrack.Uri(uri, bytes);
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
}

void Dummy::FinishAfter(uint32_t aId)
{
    cout << "Dummy::FinishAfter(" << aId << ")" << endl;
}

void Dummy::SetVolume(uint32_t aValue)
{
    cout << "Dummy::SetVolume(" << aValue << ")" << endl;
}

void Dummy::SetMute(bool aValue)
{
    cout << "Dummy::SetMute(" << aValue << ")" << endl;
}

void Dummy::SetStatusHandler(IRendererStatus& aHandler) 
{
    iStatus = &aHandler;
}

void Dummy::Prefetch()
{
    iStatus->Playing(iHandle, iId, iDuration, 128000, 24, 44100, false, "mp3");
    iTickTimer.FireIn(1000);
}

void Dummy::Tick()
{
    ++iSecond;
    iStatus->Time(iHandle, iId, iSecond);
    if(iSecond == iDuration) {
        iStatus->Finished(iHandle, iId); 
    }
    else {
        iTickTimer.FireIn(1000);
    }
}
