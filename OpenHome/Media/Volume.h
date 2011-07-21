#ifndef HEADER_OPENHOME_MEDIA_VOLUME
#define HEADER_OPENHOME_MEDIA_VOLUME

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>

#include <OpenHome/Net/Core/DvAvOpenhomeOrgVolume1.h>

namespace OpenHome {
namespace Media {

class IVolume;

class ProviderVolume : public Net::DvProviderAvOpenhomeOrgVolume1
{
public:
	ProviderVolume(Net::DvDevice& aDevice, IVolume& aVolume);

    //From DvProviderAvOpenhomeOrgVolume1 
private:
    virtual void Characteristics(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aVolumeMax, Net::IInvocationResponseUint& aVolumeUnity, Net::IInvocationResponseUint& aVolumeSteps, Net::IInvocationResponseUint& aVolumeMilliDbPerStep, Net::IInvocationResponseUint& aBalanceMax, Net::IInvocationResponseUint& aFadeMax);
    virtual void SetVolume(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void VolumeInc(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void VolumeDec(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void Volume(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
    virtual void SetMute(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue);
    virtual void Mute(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue);
    virtual void VolumeLimit(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
private:
    void SetVolumeLocked(uint32_t aValue);
private:
    Mutex iMutex;
    IVolume& iVolume;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_VOLUME


