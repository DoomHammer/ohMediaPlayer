#include "Time.h"
#include <Debug.h>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

ProviderTime::ProviderTime(Net::DvDevice& aDevice)
    : DvProviderAvOpenhomeOrgTime1(aDevice)
    , iMutex("TIME")
{
    EnableActionTime();

    SetPropertyTrackCount(0);
    SetPropertyDuration(0);
    SetPropertySeconds(0);
}

void ProviderTime::SetDuration(TUint aDuration)
{
    iMutex.Wait();

    SetPropertyDuration(aDuration);

    TUint trackCount;
    GetPropertyTrackCount(trackCount);
    trackCount++;
    SetPropertyTrackCount(trackCount);

    iMutex.Signal();
}

void ProviderTime::SetSeconds(TUint aSeconds)
{
    SetPropertySeconds(aSeconds);
}

//From DvProviderAvOpenhomeOrgTime1
void ProviderTime::Time(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aTrackCount, Net::IInvocationResponseUint& aDuration, Net::IInvocationResponseUint& aSeconds)
{
    TUint trackCount;
    TUint duration;
    TUint seconds;

    iMutex.Wait();
    GetPropertyTrackCount(trackCount);
    GetPropertyDuration(duration);
    GetPropertySeconds(seconds);
    iMutex.Signal();

    aResponse.Start();

    aTrackCount.Write(trackCount);
    aDuration.Write(duration);
    aSeconds.Write(seconds);

    aResponse.End();
    
}

