#include "Volume.h"

#include "Player.h"

using namespace OpenHome;
using namespace OpenHome::Media;

ProviderVolume::ProviderVolume(Net::DvDevice& aDevice, IVolume& aVolume)
    : DvProviderAvOpenhomeOrgVolume1(aDevice)
    , iMutex("VOLU")
    , iVolume(aVolume)
{
    PropertiesLock();
    SetPropertyVolume(50);
    SetPropertyMute(false);
    SetPropertyBalance(0);
    SetPropertyFade(0);
    SetPropertyVolumeLimit(100);
    SetPropertyVolumeMax(100);
    SetPropertyVolumeUnity(80);
    SetPropertyVolumeSteps(100);
    SetPropertyVolumeMilliDbPerStep(1024);
    SetPropertyBalanceMax(0);
    SetPropertyFadeMax(0);
    PropertiesUnlock();
    
    EnableActionCharacteristics();
    EnableActionSetVolume();
    EnableActionVolumeInc();
    EnableActionVolumeDec();
    EnableActionVolume();
    EnableActionSetMute();
    EnableActionMute();
    EnableActionVolumeLimit();
}

//From DvProviderAvOpenhomeOrgVolume1
void ProviderVolume::Characteristics(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aVolumeMax, Net::IInvocationResponseUint& aVolumeUnity, Net::IInvocationResponseUint& aVolumeSteps, Net::IInvocationResponseUint& aVolumeMilliDbPerStep, Net::IInvocationResponseUint& aBalanceMax, Net::IInvocationResponseUint& aFadeMax)
{
    //All the characteristics are readonly, therefore no mutex
    uint32_t max;
    GetPropertyVolumeMax(max);

    uint32_t unity;
    GetPropertyVolumeUnity(unity);

    uint32_t steps;
    GetPropertyVolumeSteps(steps);

    uint32_t milliDb;
    GetPropertyVolumeMilliDbPerStep(milliDb);

    uint32_t balanceMax;
    GetPropertyBalanceMax(balanceMax);

    uint32_t fadeMax;
    GetPropertyFadeMax(fadeMax);

    aResponse.Start();
    aVolumeMax.Write(max);
    aVolumeUnity.Write(unity);
    aVolumeSteps.Write(steps);
    aVolumeMilliDbPerStep.Write(milliDb);
    aBalanceMax.Write(balanceMax);
    aFadeMax.Write(fadeMax);
    aResponse.End();
}

void ProviderVolume::SetVolume(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();
    SetVolumeLocked(aValue);
    iMutex.Signal();
}

void ProviderVolume::SetVolumeLocked(TUint aValue)
{
    uint32_t limit;
    GetPropertyVolumeLimit(limit);

    if(aValue > limit) {
        aValue = limit;
    }
    SetPropertyVolume(aValue);
    iVolume.SetVolume(aValue);
}

void ProviderVolume::VolumeInc(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();
    uint32_t volume;
    GetPropertyVolume(volume);
    SetVolumeLocked(++volume);
    iMutex.Signal();
}

void ProviderVolume::VolumeDec(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();
    uint32_t volume;
    GetPropertyVolume(volume);
    SetVolumeLocked(--volume);
    iMutex.Signal();
}

void ProviderVolume::Volume(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    iMutex.Wait();

    uint32_t volume;
    GetPropertyVolume(volume);

    iMutex.Signal();

    aResponse.Start();
    aValue.Write(volume);
    aResponse.End();
}

void ProviderVolume::SetMute(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    SetPropertyMute(aValue);
    iVolume.SetMute(aValue);

    iMutex.Signal();
}

void ProviderVolume::Mute(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    iMutex.Wait();

    bool mute;
    GetPropertyMute(mute);

    iMutex.Signal();

    aResponse.Start();
    aValue.Write(mute);
    aResponse.End();
}

void ProviderVolume::VolumeLimit(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    //Limit is readonly, therefore no mutex
    uint32_t limit;
    GetPropertyVolumeLimit(limit);

    aResponse.Start();
    aValue.Write(limit);
    aResponse.End();
}
