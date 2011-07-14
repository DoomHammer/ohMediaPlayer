#ifndef HEADER_OPENHOME_MEDIAPLAYER_VOLUME
#define HEADER_OPENHOME_MEDIAPLAYER_VOLUME

#include <OhNetTypes.h>
#include <Buffer.h>
#include <Core/DvDevice.h>
#include <Thread.h>

#include <Core/DvAvOpenhomeOrgVolume1.h>

namespace OpenHome {
namespace MediaPlayer {

class ProviderVolume : public Net::DvProviderAvOpenhomeOrgVolume1
{
public:
	ProviderVolume(Net::DvDevice& aDevice);

    //From DvProviderAvOpenhomeOrgVolume1 
private:
    virtual void Characteristics(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseUint& aVolumeMax, IInvocationResponseUint& aVolumeUnity, IInvocationResponseUint& aVolumeSteps, IInvocationResponseUint& aVolumeMilliDbPerStep, IInvocationResponseUint& aBalanceMax, IInvocationResponseUint& aFadeMax);
    virtual void SetVolume(IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void VolumeInc(IInvocationResponse& aResponse, TUint aVersion);
    virtual void VolumeDec(IInvocationResponse& aResponse, TUint aVersion);
    virtual void Volume(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseUint& aValue);
    virtual void SetBalance(IInvocationResponse& aResponse, TUint aVersion, TInt aValue);
    virtual void BalanceInc(IInvocationResponse& aResponse, TUint aVersion);
    virtual void BalanceDec(IInvocationResponse& aResponse, TUint aVersion);
    virtual void Balance(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseInt& aValue);
    virtual void SetFade(IInvocationResponse& aResponse, TUint aVersion, TInt aValue);
    virtual void FadeInc(IInvocationResponse& aResponse, TUint aVersion);
    virtual void FadeDec(IInvocationResponse& aResponse, TUint aVersion);
    virtual void Fade(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseInt& aValue);
    virtual void SetMute(IInvocationResponse& aResponse, TUint aVersion, TBool aValue);
    virtual void Mute(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseBool& aValue);
    virtual void Mute(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseBool& aValue);
private:
    Mutex iMutex;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_VOLUME


