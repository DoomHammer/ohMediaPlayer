#include "Source.h"
#include <Debug.h>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Source

Source::Source(const Brx& aSystemName, const Brx& aType, const Brx& aName, TBool aVisible)
    : iSystemName(aSystemName)
    , iType(aType)
    , iName(aName)
    , iVisible(aVisible)
    , iLockable(0)
    , iHandle(-1)
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

uint32_t Source::Handle() const
{
    return iHandle;
}
