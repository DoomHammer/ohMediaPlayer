#include "Dummy.h"
#include <Debug.h>

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

void Dummy::Play(uint32_t aHandle, uint32_t aId, const uint8_t aUri[], uint32_t aUriBytes, uint32_t aSecond)
{
    iHandle = aHandle;
    iId = aId;
    iUri.assign((const char*)aUri, aUriBytes);
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
