#include <OpenHome/Media/Info.h>
#include <OpenHome/Private/Debug.h>
#include <OpenHome/Media/Player.h>

using namespace OpenHome;
using namespace OpenHome::Media;

ProviderInfo::ProviderInfo(Net::DvDevice& aDevice)
    : DvProviderAvOpenhomeOrgInfo1(aDevice)
    , iMutex("INFO")
{
    EnableActionCounters();
    EnableActionTrack();
    EnableActionDetails();
    EnableActionMetatext();
    
    SetPropertyTrackCount(0);
    SetPropertyDetailsCount(0);
    SetPropertyMetatextCount(0);
    SetPropertyUri(Brx::Empty());
    SetPropertyMetadata(Brx::Empty());
    SetPropertyDuration(0);
    SetPropertyBitRate(0);
    SetPropertyBitDepth(0);
    SetPropertySampleRate(0);
    SetPropertyLossless(false);
    SetPropertyCodecName(Brx::Empty());
    SetPropertyMetatext(Brx::Empty());
}

void ProviderInfo::SetTrack(const class Track& aTrack)
{
    iMutex.Wait();


    TUint trackCount;
    GetPropertyTrackCount(trackCount);
    trackCount++;

    PropertiesLock();
    SetPropertyTrackCount(trackCount);
    SetPropertyUri(aTrack.Uri());
    SetPropertyMetadata(aTrack.Metadata());
    SetPropertyDetailsCount(0);
    SetPropertyMetatextCount(0);
    SetPropertyDuration(0);
    SetPropertyBitRate(0);
    SetPropertyBitDepth(0);
    SetPropertySampleRate(0);
    SetPropertyLossless(false);
    SetPropertyCodecName(Brx::Empty());
    SetPropertyMetatext(Brx::Empty());
    PropertiesUnlock();

    iMutex.Signal();
}

void ProviderInfo::SetDetails(TUint aDuration, TUint aBitRate, TUint aBitDepth, TUint aSampleRate, TBool aLossless, const Brx& aCodecName)
{
    iMutex.Wait();
    
    TUint detailsCount;
    GetPropertyDetailsCount(detailsCount);
    detailsCount++;

    PropertiesLock();
    SetPropertyDetailsCount(detailsCount);
    SetPropertyDuration(aDuration);
    SetPropertyBitRate(aBitRate);
    SetPropertyBitDepth(aBitDepth);
    SetPropertySampleRate(aSampleRate);
    SetPropertyLossless(aLossless);
    SetPropertyCodecName(aCodecName);
    PropertiesUnlock();

    iMutex.Signal();
}

void ProviderInfo::SetMetatext(const Brx& aMetatext)
{
    iMutex.Wait();

    TUint metatextCount;
    GetPropertyMetatextCount(metatextCount);
    metatextCount++;

    PropertiesLock();
    SetPropertyMetatextCount(metatextCount);
    SetPropertyMetatext(aMetatext);    
    PropertiesUnlock();

    iMutex.Signal();
}

//From DvProviderAvOpenhomeOrgInfo1
void ProviderInfo::Counters(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aTrackCount, Net::IInvocationResponseUint& aDetailsCount, Net::IInvocationResponseUint& aMetatextCount)
{
    TUint trackCount;
    TUint detailsCount;
    TUint metatextCount;

    iMutex.Wait();
    GetPropertyTrackCount(trackCount);
    GetPropertyDetailsCount(detailsCount);
    GetPropertyMetatextCount(metatextCount);
    iMutex.Signal();

    aResponse.Start();

    aTrackCount.Write(trackCount);
    aDetailsCount.Write(detailsCount);
    aMetatextCount.Write(metatextCount);

    aResponse.End();
    
}

void ProviderInfo::Track(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata)
{
    Brhz uri;
    Brhz metadata;

    iMutex.Wait();
    GetPropertyUri(uri);
    GetPropertyMetadata(metadata);
    iMutex.Signal();

    aResponse.Start();

    aUri.Write(uri);
    aUri.WriteFlush();
    aMetadata.Write(metadata);
    aMetadata.WriteFlush();

    aResponse.End();
}

void ProviderInfo::Details(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aDuration, Net::IInvocationResponseUint& aBitRate, Net::IInvocationResponseUint& aBitDepth, Net::IInvocationResponseUint& aSampleRate, Net::IInvocationResponseBool& aLossless, Net::IInvocationResponseString& aCodecName)
{
    TUint duration;
    TUint bitRate;
    TUint bitDepth;
    TUint sampleRate;
    TBool lossless;
    Brhz  codecName;

    iMutex.Wait();
    GetPropertyDuration(duration);
    GetPropertyBitRate(bitRate);
    GetPropertyBitDepth(bitDepth);
    GetPropertySampleRate(sampleRate);
    GetPropertyLossless(lossless);
    GetPropertyCodecName(codecName);
    iMutex.Signal();

    aResponse.Start();

    aDuration.Write(duration);
    aBitRate.Write(bitRate);
    aBitDepth.Write(bitDepth);
    aSampleRate.Write(sampleRate);
    aLossless.Write(lossless);
    aCodecName.Write(codecName);
    aCodecName.WriteFlush();

    aResponse.End();
}

void ProviderInfo::Metatext(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Brhz metatext;

    GetPropertyMetatext(metatext);

    aResponse.Start();

    aValue.Write(metatext);
    aValue.WriteFlush();

    aResponse.End();
}
