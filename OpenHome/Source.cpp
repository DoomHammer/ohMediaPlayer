#include "Source.h"
#include "Player.h"
#include <Debug.h>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Source

Source::Source(const Brx& aSystemName, const Brx& aType, const Brx& aName, TBool aVisible, IPlayer& aPlayer)
    : iSystemName(aSystemName)
    , iType(aType)
    , iName(aName)
    , iVisible(aVisible)
    , iLockable(0)
    , iHandle(-1)
    , iPlayer(aPlayer)
{
}

void Source::SetModifiable(ILockable& aLockable, IObserver& aObserver)
{
    iLockable = &aLockable;
    RegisterObserver(aObserver);
    InformObservers();
}

TBool Source::Details(Bwx& aSystemName, Bwx& aType, Bwx& aName)
{
	aSystemName.Replace(iSystemName);
	aType.Replace(iType);

    if(iLockable) {
        iLockable->Wait();
    }
    aName.Replace(iName);
    TBool visible = iVisible;

    if(iLockable) {
        iLockable->Signal();
    }
	return (visible);
}

void Source::SetName(const Brx& aValue)
{
    ASSERT(iLockable);
	iLockable->Wait();
	iName.Replace(aValue);
	iLockable->Signal();
	InformObservers();
}

void Source::SetVisible(TBool aValue)
{
    ASSERT(iLockable);
	iLockable->Wait();
	iVisible = aValue;
	iLockable->Signal();
	InformObservers();
}

void Source::SetHandle(uint32_t aHandle)
{
    iHandle = aHandle;    
}

void Source::Play(uint32_t aId, const Brx& aUri, uint32_t aSecond)
{
    iPlayer.Play(iHandle, aId, aUri, aSecond);
}

void Source::Pause()
{
    iPlayer.Pause();
}

void Source::Unpause()
{
    iPlayer.Unpause();
}

void Source::Stop()
{
    iPlayer.Stop();
}

void Source::Deleted(uint32_t aId)
{
    iPlayer.Deleted(aId);
}

uint32_t Source::NewId()
{
    return iPlayer.NewId();
}
