#ifndef HEADER_OPENHOME_MEDIA_PRODUCT
#define HEADER_OPENHOME_MEDIA_PRODUCT

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Private/Timer.h>

#include <Generated/DvAvOpenhomeOrgProduct1.h>

#include <OpenHome/Media/Source.h>
#include <OpenHome/Media/Player.h>

namespace OpenHome {
namespace Media {

class ProviderProduct : public Net::DvProviderAvOpenhomeOrgProduct1, public IObserver, public ILockable
{
	static const TUint kMaxRoomBytes = 30;
    static const TUint kMaxSourceXmlBytes = 4000;

public:
	ProviderProduct(Net::DvDevice& aDevice
        , IStandbyHandler& aStandbyHandler
        , ISourceIndexHandler& aSourceIndexHandler
        , TBool aStandby
        , const TChar* aAtrributes
        , const TChar* aManufacturerName
        , const TChar* aManufacturerInfo
        , const TChar* aManufacturerUrl
        , const TChar* aManufacturerImageUri
        , const TChar* aModelName
        , const TChar* aModelInfo
        , const TChar* aModelUrl
        , const TChar* aModelImageUri
        , const TChar* aProductRoom
        , const TChar* aProductName
        , const TChar* aProductInfo
        , const TChar* aProductUrl
        , const TChar* aProductImageUri);
    
    uint32_t AddSource(class Source* aSource);
    class Source& GetSource(uint32_t aIndex);

private:
    void UpdateSourceXml();

    //From IObserver
private:
    virtual void ObservableChanged();

    //From ILockable
private:
    virtual void Wait() const;
    virtual void Signal() const;
    
    //From DvProviderAvOpenhomeOrgProduct1 
private:
    virtual void Manufacturer(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri);
    virtual void Model(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri);
    virtual void Product(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseString& aRoom, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri);
    virtual void Standby(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseBool& aValue);
    virtual void SetStandby(Net::IDvInvocation& aResponse,  TBool aValue);
    virtual void SourceCount(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseUint& aValue);
    virtual void SourceXml(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseString& aValue);
    virtual void SourceIndex(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseUint& aValue);
    virtual void SetSourceIndex(Net::IDvInvocation& aResponse,  TUint aValue);
    virtual void SetSourceIndexByName(Net::IDvInvocation& aResponse,  const Brx& aValue);
    virtual void Source(Net::IDvInvocation& aResponse,  TUint aIndex, Net::IDvInvocationResponseString& aSystemName, Net::IDvInvocationResponseString& aType, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseBool& aVisible);
    virtual void Attributes(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseString& aValue);
    virtual void SourceXmlChangeCount(Net::IDvInvocation& aResponse,  Net::IDvInvocationResponseUint& aValue);

private:
    IStandbyHandler& iStandbyHandler;
    ISourceIndexHandler& iSourceIndexHandler;
    TUint iSourceXmlChangeCount;
	mutable Mutex iMutex;
    std::vector<class Source*> iSourceList;
    Bws<kMaxSourceXmlBytes> iSourceXml;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_PRODUCT

