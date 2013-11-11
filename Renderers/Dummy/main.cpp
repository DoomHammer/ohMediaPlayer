#include <stdio.h>
#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Net/Core/OhNet.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/OptionParser.h>
#include <OpenHome/Media/Product.h>
#include <OpenHome/Media/Playlist.h>
#include "Dummy.h"

using namespace OpenHome;
using namespace OpenHome::Media;
using namespace OpenHome::TestFramework;

static const TUint kTracksMax = 1000;
static const char* kProtocolInfo = "http-get:*:audio/x-flac:*,http-get:*:audio/wav:*,http-get:*:audio/wave:*,http-get:*:audio/x-wav:*,http-get:*:audio/mpeg:*,http-get:*:audio/x-mpeg:*,http-get:*:audio/mp1:*,http-get:*:audio/aiff:*,http-get:*:audio/x-aiff:*,http-get:*:audio/x-m4a:*,http-get:*:audio/x-ms-wma:*,rtsp-rtp-udp:*:audio/x-ms-wma:*,http-get:*:audio/x-scpls:*,http-get:*:audio/x-mpegurl:*,http-get:*:audio/x-ms-asf:*,http-get:*:audio/x-ms-wax:*,http-get:*:audio/x-ms-wvx:*,http-get:*:video/x-ms-asf:*,http-get:*:video/x-ms-wax:*,http-get:*:video/x-ms-wvx:*,http-get:*:text/xml:*,http-get:*:audio/aac:*,http-get:*:audio/aacp:*,http-get:*:audio/mp4:*,http-get:*:audio/ogg:*,http-get:*:audio/x-ogg:*,http-get:*:application/ogg:*";

class StandbyHandler : public IStandbyHandler
{
public: 
    virtual void SetStandby(TBool aValue);
};

void StandbyHandler::SetStandby(TBool aValue)
{
    printf("Setting Standby: %d\n", aValue);
}

class SourceIndexHandler : public ISourceIndexHandler
{
public:
    virtual void SetSourceIndex(TUint aValue);
};

void SourceIndexHandler::SetSourceIndex(TUint aValue)
{
    printf("Setting source: %d\n", aValue);
}

int main(int aArgc, char* aArgv[])
{
    Net::InitialisationParams* initParams = Net::InitialisationParams::Create();

    Net::Library* lib = new Net::Library(initParams);

    Net::DvStack* dvStack = lib->StartDv();

	Brhz udn("device");

    Net::DvDeviceStandard* device = new Net::DvDeviceStandard(*dvStack, udn);

    SourceIndexHandler* sourceIndexHandler = new SourceIndexHandler();
    StandbyHandler* standbyHandler = new StandbyHandler();

    Dummy* dummy = new Dummy(lib->Env());

    Player* player = new Player(
        dummy,
        *device, 
        *standbyHandler, 
        *sourceIndexHandler, 
        true,
        "Info Time Volume",
        "OpenHome",
        "OpenHome Consortium",
        "http://openhome.org",
        "",
        "OpenHome Media Player",
        "",
        "",
        "",
        "OpenHomeKeith",
        "OpenHome Media Player",
        "",
        "",
        "");

    SourcePlaylist* sourcePlaylist = new SourcePlaylist(*device, kTracksMax, kProtocolInfo, *player);

    player->AddSource(sourcePlaylist);
    

    device->SetEnabled();

    char c = '\0';
    while (c != 'q') {
        scanf("%c", &c);
    }

	printf("Quiting...\n");

    delete (device);
    delete sourceIndexHandler;
    delete standbyHandler;
    delete player;
    delete lib;

    printf("Exit complete\n");
	
    return (0);
}
