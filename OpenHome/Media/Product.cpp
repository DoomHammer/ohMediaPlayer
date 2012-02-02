#include <OpenHome/Media/Product.h>
#include <OpenHome/Private/Debug.h>

using namespace OpenHome;
using namespace OpenHome::Media;

// Product

ProviderProduct::ProviderProduct(Net::DvDevice& aDevice
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

uint32_t ProviderProduct::AddSource(class Source* aSource)
{
    iSourceList.push_back(aSource);
    aSource->SetModifiable(*this, *this);

    TUint count(iSourceList.size());
    SetPropertySourceCount(count);
    Log::Print("Added source, source count now: %d\n", count);
    return count-1;
}

Source& ProviderProduct::GetSource(uint32_t aIndex)
{
    ASSERT(aIndex < iSourceList.size());
    return *(iSourceList[aIndex]);
}

void ProviderProduct::UpdateSourceXml()
{
    Log::Print("ProviderProduct::UpdateSourceXml()\n");
    iSourceXml.Replace("<SourceList>");

    Wait();

	for (std::vector<Media::Source*>::const_iterator i = iSourceList.begin(); i != iSourceList.end(); ++i) {
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
void ProviderProduct::ObservableChanged()
{
    UpdateSourceXml();
}

//From ILockable
void ProviderProduct::Wait() const
{
    iMutex.Wait();
}

void ProviderProduct::Signal() const
{
    iMutex.Signal();
}

//From DvProviderAvOpenhomeOrgProduct1
void ProviderProduct::Manufacturer(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri)
{
	Brhz name;
	Brhz info;
	Brhz url;
	Brhz image;
    GetPropertyManufacturerName(name);
    GetPropertyManufacturerInfo(info);
    GetPropertyManufacturerUrl(url);
    GetPropertyManufacturerImageUri(image);
    aResponse.StartResponse();
    aName.Write(name);
    aName.WriteFlush();
    aInfo.Write(info);
    aInfo.WriteFlush();
    aUrl.Write(url);
    aUrl.WriteFlush();
	aImageUri.Write(image);
	aImageUri.WriteFlush();
    aResponse.EndResponse();
}

void ProviderProduct::Model(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri)
{
	Brhz name;
	Brhz info;
	Brhz url;
	Brhz image;
    GetPropertyModelName(name);
    GetPropertyModelInfo(info);
    GetPropertyModelUrl(url);
    GetPropertyModelImageUri(image);
    aResponse.StartResponse();
    aName.Write(name);
    aName.WriteFlush();
    aInfo.Write(info);
    aInfo.WriteFlush();
    aUrl.Write(url);
    aUrl.WriteFlush();
	aImageUri.Write(image);
	aImageUri.WriteFlush();
    aResponse.EndResponse();
}

void ProviderProduct::Product(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aRoom, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseString& aInfo, Net::IDvInvocationResponseString& aUrl, Net::IDvInvocationResponseString& aImageUri)
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
    aResponse.StartResponse();
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
    aResponse.EndResponse();
}

void ProviderProduct::Standby(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue)
{
	TBool value;
    GetPropertyStandby(value);
    aResponse.StartResponse();
    aValue.Write(value);
    aResponse.EndResponse();
}

void ProviderProduct::SetStandby(Net::IDvInvocation& aResponse, TBool aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    if (SetPropertyStandby(aValue)) {
    	iStandbyHandler.SetStandby(aValue);
    }
}

void ProviderProduct::SourceCount(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
	TUint value;
    GetPropertySourceCount(value);
    aResponse.StartResponse();
    aValue.Write(value);
    aResponse.EndResponse();
}

void ProviderProduct::SourceXml(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue)
{
	Brhz value;
    GetPropertySourceXml(value);
    aResponse.StartResponse();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.EndResponse();
}

void ProviderProduct::SourceIndex(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
	TUint value;
    GetPropertySourceIndex(value);
    aResponse.StartResponse();
    aValue.Write(value);
    aResponse.EndResponse();
}

void ProviderProduct::SetSourceIndex(Net::IDvInvocation& aResponse, TUint aValue)
{
	TUint count;
    GetPropertySourceCount(count);
	if (aValue < count) {
	    aResponse.StartResponse();
	    aResponse.EndResponse();
	    if (SetPropertySourceIndex(aValue)) {
	    	iSourceIndexHandler.SetSourceIndex(aValue);
	    }
	}
	else {
		aResponse.Error(802, Brn("Source index out of range"));
	}
}

void ProviderProduct::SetSourceIndexByName(Net::IDvInvocation& aResponse, const Brx& aValue)
{
}

void ProviderProduct::Source(Net::IDvInvocation& aResponse, TUint aIndex, Net::IDvInvocationResponseString& aSystemName, Net::IDvInvocationResponseString& aType, Net::IDvInvocationResponseString& aName, Net::IDvInvocationResponseBool& aVisible)
{
    //TODO: Missing locking
	TUint count;
    GetPropertySourceCount(count);
	if (aIndex < count) {
		class Source* source = iSourceList[aIndex];
	    aResponse.StartResponse();
	    aSystemName.Write(source->iSystemName);
	    aSystemName.WriteFlush();
	    aType.Write(source->iType);
	    aType.WriteFlush();
	    aName.Write(source->iName);
	    aName.WriteFlush();
	    aVisible.Write(source->iVisible);
	    aResponse.EndResponse();
	}
	else {
		aResponse.Error(802, Brn("Source index out of range"));
	}
}

void ProviderProduct::Attributes(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue)
{
	Brhz value;
    GetPropertyAttributes(value);
    aResponse.StartResponse();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.EndResponse();
}

void ProviderProduct::SourceXmlChangeCount(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
	iMutex.Wait();
	TUint value = iSourceXmlChangeCount;
	iMutex.Signal();
    aResponse.StartResponse();
    aValue.Write(value);
    aResponse.EndResponse();
}


