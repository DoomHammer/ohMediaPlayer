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
    virtual void Counters(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aTrackCount, Net::IInvocationResponseUint& aDetailsCount, Net::IInvocationResponseUint& aMetatextCount);
    virtual void Track(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata);
    virtual void Details(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aDuration, Net::IInvocationResponseUint& aBitRate, Net::IInvocationResponseUint& aBitDepth, Net::IInvocationResponseUint& aSampleRate, Net::IInvocationResponseBool& aLossless, Net::IInvocationResponseString& aCodecName);
    virtual void Metatext(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue);

private:
    Mutex iMutex;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_INFO

