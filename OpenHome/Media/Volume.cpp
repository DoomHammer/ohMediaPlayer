#include <OpenHome/Media/Volume.h>

#include <OpenHome/Media/Player.h>

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
void ProviderVolume::Characteristics(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aVolumeMax, Net::IDvInvocationResponseUint& aVolumeUnity, Net::IDvInvocationResponseUint& aVolumeSteps, Net::IDvInvocationResponseUint& aVolumeMilliDbPerStep, Net::IDvInvocationResponseUint& aBalanceMax, Net::IDvInvocationResponseUint& aFadeMax)
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

    aResponse.StartResponse();
    aVolumeMax.Write(max);
    aVolumeUnity.Write(unity);
    aVolumeSteps.Write(steps);
    aVolumeMilliDbPerStep.Write(milliDb);
    aBalanceMax.Write(balanceMax);
    aFadeMax.Write(fadeMax);
    aResponse.EndResponse();
}

void ProviderVolume::SetVolume(Net::IDvInvocation& aResponse, TUint aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

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

void ProviderVolume::VolumeInc(Net::IDvInvocation& aResponse, TUint aVersion)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iMutex.Wait();
    uint32_t volume;
    GetPropertyVolume(volume);
    SetVolumeLocked(++volume);
    iMutex.Signal();
}

void ProviderVolume::VolumeDec(Net::IDvInvocation& aResponse, TUint aVersion)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iMutex.Wait();
    uint32_t volume;
    GetPropertyVolume(volume);
    SetVolumeLocked(--volume);
    iMutex.Signal();
}

void ProviderVolume::Volume(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
    iMutex.Wait();

    uint32_t volume;
    GetPropertyVolume(volume);

    iMutex.Signal();

    aResponse.StartResponse();
    aValue.Write(volume);
    aResponse.EndResponse();
}

void ProviderVolume::SetMute(Net::IDvInvocation& aResponse, TBool aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iMutex.Wait();

    SetPropertyMute(aValue);
    iVolume.SetMute(aValue);

    iMutex.Signal();
}

void ProviderVolume::Mute(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue)
{
    iMutex.Wait();

    bool mute;
    GetPropertyMute(mute);

    iMutex.Signal();

    aResponse.StartResponse();
    aValue.Write(mute);
    aResponse.EndResponse();
}

void ProviderVolume::VolumeLimit(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
    //Limit is readonly, therefore no mutex
    uint32_t limit;
    GetPropertyVolumeLimit(limit);

    aResponse.StartResponse();
    aValue.Write(limit);
    aResponse.EndResponse();
}
