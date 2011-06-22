#include "Product.h"
#include <Debug.h>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Observable

Observable::Observable()
{
}

void Observable::RegisterObserver(IObserver& aObserver)
{
    Log::Print("Observable::RegisterObserver()\n");
	iObserverList.push_back(&aObserver);
}   

void Observable::InformObservers() const
{
    Log::Print("Observable::InformObservers()\n");
    for (std::vector<IObserver*>::const_iterator it = iObserverList.begin(); it != iObserverList.end(); ++it) {
        (*it)->ObservableChanged();
    }	
}

// Source

Source::Source(const Brx& aSystemName, const Brx& aType, const Brx& aName, TBool aVisible, ILockable& aLockable, IObserver& aObserver)
    : iSystemName(aSystemName)
    , iType(aType)
    , iName(aName)
    , iVisible(aVisible)
    , iLockable(aLockable)
{
    RegisterObserver(aObserver);
}

TBool Source::Details(Bwx& aSystemName, Bwx& aType, Bwx& aName)
{
	aSystemName.Replace(iSystemName);
	aType.Replace(iType);
	iLockable.Wait();
	aName.Replace(iName);
	TBool visible = iVisible;
	iLockable.Signal();
	return (visible);
}

void Source::SetName(const Brx& aValue)
{
	iLockable.Wait();
	iName.Replace(aValue);
	iLockable.Signal();
	InformObservers();
}

void Source::SetVisible(TBool aValue)
{
	iLockable.Wait();
	iVisible = aValue;
	iLockable.Signal();
	InformObservers();
}

// Product

ProductImpl::ProductImpl(Net::DvDevice& aDevice
    , IStandbyHandler& aStandbyHandler
    , ISourceIndexHandler& aSourceIndexHandler
	, TBool aStandby
	, const TChar* aAttributes
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
	, const TChar* aProductImageUri)
    : DvProviderAvOpenhomeOrgProduct1(aDevice)
    , iStandbyHandler(aStandbyHandler)
    , iSourceIndexHandler(aSourceIndexHandler)
    , iSourceXmlChangeCount(0)
    , iMutex("PROD")
    , iSourceXml(Brx::Empty())
{
    aDevice.SetAttribute("Upnp.Domain", "av.openhome.org");
    aDevice.SetAttribute("Upnp.Type", "ProductImpl");
    aDevice.SetAttribute("Upnp.Version", "1");

    Bwh tmp(strlen(aProductName) + strlen(aProductRoom) + 1);
    tmp.Append(aProductRoom);
    tmp.Append(':');
    tmp.Append(aProductName);
    Brhz friendlyName;
    tmp.TransferTo(friendlyName);
    aDevice.SetAttribute("Upnp.FriendlyName", friendlyName.CString());

    aDevice.SetAttribute("Upnp.Manufacturer", aManufacturerName);
    aDevice.SetAttribute("Upnp.ManufacturerUrl", aManufacturerUrl);
    aDevice.SetAttribute("Upnp.ModelDescription", aModelInfo);
    aDevice.SetAttribute("Upnp.ModelName", aModelName);
    aDevice.SetAttribute("Upnp.ModelNumber", "");
    aDevice.SetAttribute("Upnp.ModelUrl", aModelUrl);
    aDevice.SetAttribute("Upnp.SerialNumber", "");
    aDevice.SetAttribute("Upnp.Upc", "");

    EnableActionManufacturer();
    EnableActionModel();
    EnableActionProduct();
    EnableActionStandby();
    EnableActionSetStandby();
    EnableActionSourceCount();
    EnableActionSourceXml();
    EnableActionSourceIndex();
    EnableActionSetSourceIndex();
    EnableActionSetSourceIndexByName();
    EnableActionSource();
    EnableActionAttributes();
    EnableActionSourceXmlChangeCount();
    
    SetPropertyStandby(aStandby);
    SetPropertyAttributes(Brn(aAttributes));
    
    SetPropertyManufacturerName(Brn(aManufacturerName));
    SetPropertyManufacturerInfo(Brn(aManufacturerInfo));
    SetPropertyManufacturerUrl(Brn(aManufacturerUrl));
    SetPropertyManufacturerImageUri(Brn(aManufacturerImageUri));
    
    SetPropertyModelName(Brn(aModelName));
    SetPropertyModelInfo(Brn(aModelInfo));
    SetPropertyModelUrl(Brn(aModelUrl));
    SetPropertyModelImageUri(Brn(aModelImageUri));
    
    SetPropertyProductRoom(Brn(aProductRoom));
    SetPropertyProductName(Brn(aProductName));
    SetPropertyProductInfo(Brn(aProductInfo));
    SetPropertyProductUrl(Brn(aProductUrl));
    SetPropertyProductImageUri(Brn(aProductImageUri));
    
    SetPropertySourceIndex(0);
    SetPropertySourceCount(0);
    SetPropertySourceXml(iSourceXml);
}

TUint ProductImpl::CreateSource(const Brx& aSystemName, const Brx& aType, const Brx& aName, TBool aVisible) 
{
    MediaPlayer::Source* source = new MediaPlayer::Source(aSystemName, aType, aName, aVisible, *this, *this);
    iSourceList.push_back(source); 
    ObservableChanged();
    TUint count(iSourceList.size());
    SetPropertySourceCount(count);
    Log::Print("Created source, source count now: %d\n", count);
    return count;
}

Source& ProductImpl::GetSource(TUint aIndex)
{
    ASSERT(aIndex < iSourceList.size());
    return *(iSourceList[aIndex]);
}

void ProductImpl::UpdateSourceXml()
{
    Log::Print("ProductImpl::UpdateSourceXml()\n");
    iSourceXml.Replace("<SourceList>");

    Wait();

	for (std::vector<MediaPlayer::Source*>::const_iterator i = iSourceList.begin(); i != iSourceList.end(); ++i) {
        iSourceXml.Append("<Source>");

        //TODO: Xml escape the name
        iSourceXml.Append("<Name>");
        iSourceXml.Append((*i)->iName);
        iSourceXml.Append("</Name>");

        iSourceXml.Append("<Type>");
        iSourceXml.Append((*i)->iType);
        iSourceXml.Append("</Type>");

        iSourceXml.Append("<Visible>");
        if ((*i)->iVisible) {
            iSourceXml.Append("true");
        }
        else {
            iSourceXml.Append("false");
        }
        iSourceXml.Append("</Visible>");

        iSourceXml.Append("</Source>");
    }

    iSourceXml.Append("</SourceList>");

    SetPropertySourceXml(iSourceXml);

    iSourceXmlChangeCount++;

    Signal();
}

//From IObserver
void ProductImpl::ObservableChanged()
{
    UpdateSourceXml();
}

//From ILockable
void ProductImpl::Wait() const
{
    iMutex.Wait();
}

void ProductImpl::Signal() const
{
    iMutex.Signal();
}

//From DvProviderAvOpenhomeOrgProduct1
void ProductImpl::Manufacturer(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri)
{
	Brhz name;
	Brhz info;
	Brhz url;
	Brhz image;
    GetPropertyManufacturerName(name);
    GetPropertyManufacturerInfo(info);
    GetPropertyManufacturerUrl(url);
    GetPropertyManufacturerImageUri(image);
    aResponse.Start();
    aName.Write(name);
    aName.WriteFlush();
    aInfo.Write(info);
    aInfo.WriteFlush();
    aUrl.Write(url);
    aUrl.WriteFlush();
	aImageUri.Write(image);
	aImageUri.WriteFlush();
    aResponse.End();
}

void ProductImpl::Model(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri)
{
	Brhz name;
	Brhz info;
	Brhz url;
	Brhz image;
    GetPropertyModelName(name);
    GetPropertyModelInfo(info);
    GetPropertyModelUrl(url);
    GetPropertyModelImageUri(image);
    aResponse.Start();
    aName.Write(name);
    aName.WriteFlush();
    aInfo.Write(info);
    aInfo.WriteFlush();
    aUrl.Write(url);
    aUrl.WriteFlush();
	aImageUri.Write(image);
	aImageUri.WriteFlush();
    aResponse.End();
}

void ProductImpl::Product(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aRoom, Net::IInvocationResponseString& aName, Net::IInvocationResponseString& aInfo, Net::IInvocationResponseString& aUrl, Net::IInvocationResponseString& aImageUri)
{
	Brhz room;
	Brhz name;
	Brhz info;
	Brhz url;
	Brhz image;
    GetPropertyProductRoom(room);
    GetPropertyProductName(name);
    GetPropertyProductInfo(info);
    GetPropertyProductUrl(url);
    GetPropertyProductImageUri(image);
    aResponse.Start();
    aRoom.Write(room);
    aRoom.WriteFlush();
    aName.Write(name);
    aName.WriteFlush();
    aInfo.Write(info);
    aInfo.WriteFlush();
    aUrl.Write(url);
    aUrl.WriteFlush();
	aImageUri.Write(image);
	aImageUri.WriteFlush();
    aResponse.End();
}

void ProductImpl::Standby(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
	TBool value;
    GetPropertyStandby(value);
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}

void ProductImpl::SetStandby(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    aResponse.Start();
    aResponse.End();

    if (SetPropertyStandby(aValue)) {
    	iStandbyHandler.SetStandby(aValue);
    }
}

void ProductImpl::SourceCount(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
	TUint value;
    GetPropertySourceCount(value);
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}

void ProductImpl::SourceXml(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
	Brhz value;
    GetPropertySourceXml(value);
    aResponse.Start();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.End();
}

void ProductImpl::SourceIndex(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
	TUint value;
    GetPropertySourceIndex(value);
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}

void ProductImpl::SetSourceIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
	TUint count;
    GetPropertySourceCount(count);
	if (aValue < count) {
	    aResponse.Start();
	    aResponse.End();
	    if (SetPropertySourceIndex(aValue)) {
	    	iSourceIndexHandler.SetSourceIndex(aValue);
	    }
	}
	else {
		aResponse.Error(802, Brn("Source index out of range"));
	}
}

void ProductImpl::SetSourceIndexByName(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aValue)
{
}

void ProductImpl::Source(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aIndex, Net::IInvocationResponseString& aSystemName, Net::IInvocationResponseString& aType, Net::IInvocationResponseString& aName, Net::IInvocationResponseBool& aVisible)
{
	TUint count;
    GetPropertySourceCount(count);
	if (aIndex < count) {
		class Source* source = iSourceList[aIndex];
	    aResponse.Start();
	    aSystemName.Write(source->iSystemName);
	    aSystemName.WriteFlush();
	    aType.Write(source->iType);
	    aType.WriteFlush();
	    aName.Write(source->iName);
	    aName.WriteFlush();
	    aVisible.Write(source->iVisible);
	    aResponse.End();
	}
	else {
		aResponse.Error(802, Brn("Source index out of range"));
	}
}

void ProductImpl::Attributes(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
	Brhz value;
    GetPropertyAttributes(value);
    aResponse.Start();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.End();
}

void ProductImpl::SourceXmlChangeCount(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
	iMutex.Wait();
	TUint value = iSourceXmlChangeCount;
	iMutex.Signal();
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}


