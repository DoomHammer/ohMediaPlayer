#ifndef HEADER_OPENHOME_MEDIAPLAYER_TIME
#define HEADER_OPENHOME_MEDIAPLAYER_TIME

#include <OhNetTypes.h>
#include <Buffer.h>
#include <Core/DvDevice.h>
#include <Thread.h>

#include <Core/DvAvOpenhomeOrgTime1.h>

namespace OpenHome {
namespace MediaPlayer {

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
    virtual void Time(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aTrackCount, Net::IInvocationResponseUint& aDuration, Net::IInvocationResponseUint& aSeconds);

private:
    Mutex iMutex;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_TIME

