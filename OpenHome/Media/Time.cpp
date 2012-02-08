#include <OpenHome/Media/Time.h>
#include <OpenHome/Private/Debug.h>

using namespace OpenHome;
using namespace OpenHome::Media;

ProviderTime::ProviderTime(Net::DvDevice& aDevice)
    : DvProviderAvOpenhomeOrgTime1(aDevice)
    , iMutex("TIME")
{
    EnablePropertyTrackCount();
    EnablePropertyDuration();
    EnablePropertySeconds();

    EnableActionTime();

    SetPropertyTrackCount(0);
    SetPropertyDuration(0);
    SetPropertySeconds(0);
}

void ProviderTime::SetDuration(TUint aDuration)
{
    iMutex.Wait();

    TUint trackCount;
    GetPropertyTrackCount(trackCount);
    trackCount++;

    PropertiesLock();
    SetPropertyTrackCount(trackCount);
    SetPropertyDuration(aDuration);
    PropertiesUnlock();

    iMutex.Signal();
}

void ProviderTime::SetSeconds(TUint aSeconds)
{
    SetPropertySeconds(aSeconds);
}

TUint ProviderTime::Seconds()
{
    TUint seconds;
    GetPropertySeconds(seconds);
    return seconds;
}

TUint ProviderTime::Duration()
{
    TUint duration;
    GetPropertyDuration(duration);
    return duration;
}

//From DvProviderAvOpenhomeOrgTime1
void ProviderTime::Time(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aTrackCount, Net::IDvInvocationResponseUint& aDuration, Net::IDvInvocationResponseUint& aSeconds)
{
    TUint trackCount;
    TUint duration;
    TUint seconds;

    iMutex.Wait();
    GetPropertyTrackCount(trackCount);
    GetPropertyDuration(duration);
    GetPropertySeconds(seconds);
    iMutex.Signal();

    aResponse.StartResponse();

    aTrackCount.Write(trackCount);
    aDuration.Write(duration);
    aSeconds.Write(seconds);

    aResponse.EndResponse();
    
}

