#ifndef HEADER_OPENHOME_MEDIA_TIME
#define HEADER_OPENHOME_MEDIA_TIME

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>

#include <OpenHome/Net/Core/DvAvOpenhomeOrgTime1.h>

namespace OpenHome {
namespace Media {

class ProviderTime : public Net::DvProviderAvOpenhomeOrgTime1
{
public:
	ProviderTime(Net::DvDevice& aDevice);

    void SetDuration(TUint aDuration);
    void SetSeconds(TUint aSeconds);
    TUint Seconds();
    TUint Duration();
    
    //From DvProviderAvOpenhomeOrgTime1 
private:
    virtual void Time(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aTrackCount, Net::IDvInvocationResponseUint& aDuration, Net::IDvInvocationResponseUint& aSeconds);

private:
    Mutex iMutex;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_TIME

