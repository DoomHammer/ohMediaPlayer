#include "Volume.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

ProviderVolume::ProviderVolume(Net::DvDevice& aDevice)
    : DvProviderAvOpenhomeOrgVolume1(aDevice)
    , iMutex("VOLU")
{
    SetPropertyVolume(50);
    SetPropertyMute(false);
    SetPropertyBalance(0);
    SetPropertyFade(0);
    SetPropertyVolumeLimit(100);
    SetPropertyVolumeMax(100);
    SetPropertyVolumeUnity(80);
    SetPropertyVolumeSteps(100);
    SetPropertyVolumeMillDbPerStep(1024);
    SetPropertyBalanceMax(15);
    SetPropertyFadeMax(0);
    
    EnableActionCharacteristics();
    EnableActionSetVolume();
    EnableActionVolumeInc();
    EnableActionVolumeDec();
    EnableActionVolume();
    EnableActionSetBalance();
    EnableActionBalanceInc();
    EnableActionBalanceDec();
    EnableActionBalance();
    EnableActionSetFade();
    EnableActionFadeInc();
    EnableActionFadeDec();
    EnableActionFade();
    EnableActionSetMute();
    EnableActionMute();
    EnableActionVolumeLimit();
}

//From DvProviderAvOpenhomeOrgVolume1
void ProviderVolume::Characteristics(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseUint& aVolumeMax, IInvocationResponseUint& aVolumeUnity, IInvocationResponseUint& aVolumeSteps, IInvocationResponseUint& aVolumeMilliDbPerStep, IInvocationResponseUint& aBalanceMax, IInvocationResponseUint& aFadeMax)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::SetVolume(IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::VolumeInc(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::VolumeDec(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::Volume(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseUint& aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::SetBalance(IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::BalanceInc(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::BalanceDec(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::Balance(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseInt& aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::SetFade(IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::FadeInc(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::FadeDec(IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::Fade(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseInt& aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::SetMute(IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::Mute(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseBool& aValue)
{
    aResponse.Start();
    aResponse.Stop();
}

void ProviderVolume::VolumeLimit(IInvocationResponse& aResponse, TUint aVersion, IInvocationResponseUint& aValue)
{
    aResponse.Start();
    aResponse.Stop();
}
