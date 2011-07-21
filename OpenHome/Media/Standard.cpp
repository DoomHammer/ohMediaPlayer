#include <OpenHome/Media/Standard.h>
#include <OpenHome/Private/Debug.h>

using namespace OpenHome;
using namespace OpenHome::Media;

// Observable

Observable::Observable()
{
}

void Observable::RegisterObserver(IObserver& aObserver)
{
    Log::Print("Observable::RegisterObserver()\n");
	iObserverList.push_back(&aObserver);
}   

void Observable::InformObservers() const
{
    Log::Print("Observable::InformObservers()\n");
    for (std::vector<IObserver*>::const_iterator it = iObserverList.begin(); it != iObserverList.end(); ++it) {
        (*it)->ObservableChanged();
    }	
}

