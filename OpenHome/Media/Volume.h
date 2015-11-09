#ifndef HEADER_OPENHOME_MEDIA_VOLUME
#define HEADER_OPENHOME_MEDIA_VOLUME

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>

#include <Generated/DvAvOpenhomeOrgVolume1.h>

namespace OpenHome {
namespace Media {

class IVolume;

class ProviderVolume : public Net::DvProviderAvOpenhomeOrgVolume1
{
public:
	ProviderVolume(Net::DvDevice& aDevice, IVolume& aVolume);

    //From DvProviderAvOpenhomeOrgVolume1 
private:
    virtual void Characteristics(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aVolumeMax, Net::IDvInvocationResponseUint& aVolumeUnity, Net::IDvInvocationResponseUint& aVolumeSteps, Net::IDvInvocationResponseUint& aVolumeMilliDbPerStep, Net::IDvInvocationResponseUint& aBalanceMax, Net::IDvInvocationResponseUint& aFadeMax);
    virtual void SetVolume(Net::IDvInvocation& aResponse, TUint aValue);
    virtual void VolumeInc(Net::IDvInvocation& aResponse);
    virtual void VolumeDec(Net::IDvInvocation& aResponse);
    virtual void Volume(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue);
    virtual void SetMute(Net::IDvInvocation& aResponse, TBool aValue);
    virtual void Mute(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue);
    virtual void VolumeLimit(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue);
private:
    void SetVolumeLocked(uint32_t aValue);
private:
    Mutex iMutex;
    IVolume& iVolume;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_VOLUME


