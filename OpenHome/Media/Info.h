#ifndef HEADER_OPENHOME_MEDIA_INFO
#define HEADER_OPENHOME_MEDIA_INFO

#include <OpenHome/Net/Core/DvDevice.h>

#include <OpenHome/Net/Core/DvAvOpenhomeOrgInfo1.h>

namespace OpenHome {
namespace Media {

class Track;

class ProviderInfo : public Net::DvProviderAvOpenhomeOrgInfo1
{
public:
	ProviderInfo(Net::DvDevice& aDevice);

    void SetTrack(const class Track& aTrack);
    void SetDetails(TUint aDuration, TUint aBitRate, TUint aBitDepth, TUint aSampleRate, TBool aLossless, const Brx& aCodecName);
    void SetMetatext(const Brx& aMetatext);
    
    //From DvProviderAvOpenhomeOrgInfo1 
private:
    virtual void Counters(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aTrackCount, Net::IDvInvocationResponseUint& aDetailsCount, Net::IDvInvocationResponseUint& aMetatextCount);
    virtual void Track(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aUri, Net::IDvInvocationResponseString& aMetadata);
    virtual void Details(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aDuration, Net::IDvInvocationResponseUint& aBitRate, Net::IDvInvocationResponseUint& aBitDepth, Net::IDvInvocationResponseUint& aSampleRate, Net::IDvInvocationResponseBool& aLossless, Net::IDvInvocationResponseString& aCodecName);
    virtual void Metatext(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue);

private:
    Mutex iMutex;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_INFO

