#ifndef HEADER_OPENHOME_MEDIAPLAYER_PRODUCT
#define HEADER_OPENHOME_MEDIAPLAYER_PRODUCT

#include <OhNetTypes.h>
#include <Buffer.h>
#include <Core/DvDevice.h>
#include <Thread.h>
#include <Timer.h>

#include <Core/DvAvOpenhomeOrgProduct1.h>

#include "Source.h"
#include "Player.h"

namespace OpenHome {
namespace MediaPlayer {

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
    virtual void Manufacturer(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri);
    virtual void Model(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri);
    virtual void Product(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aRoom, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri);
    virtual void Standby(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue);
    virtual void SetStandby(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue);
    virtual void SourceCount(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
    virtual void SourceXml(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue);
    virtual void SourceIndex(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
    virtual void SetSourceIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void SetSourceIndexByName(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aValue);
    virtual void Source(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aIndex, Net::IInvocationResponseString& aSystemName, Net::IInvocationResponseString& aType, Net::IInvocationResponseString& aName, Net::IInvocationResponseBool& aVisible);
    virtual void Attributes(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue);
    virtual void SourceXmlChangeCount(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);

private:
    IStandbyHandler& iStandbyHandler;
    ISourceIndexHandler& iSourceIndexHandler;
    TUint iSourceXmlChangeCount;
	mutable Mutex iMutex;
    std::vector<class Source*> iSourceList;
    Bws<kMaxSourceXmlBytes> iSourceXml;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_PRODUCT

