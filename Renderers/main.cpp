#include <stdio.h>
#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Net/Core/OhNet.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Debug.h>
#include <OpenHome/Private/OptionParser.h>
#include <OpenHome/Media/Product.h>
#include <OpenHome/Media/Playlist.h>


#include <Config/Config.h>

#include "config.h"

#include "Dummy/Dummy.h"
#ifdef HAVE_VLC_VLC_H
#  include "Vlc/Vlc.h"
#endif

using namespace OpenHome;
using namespace OpenHome::Media;
using namespace OpenHome::TestFramework;

static const TUint kTracksMax = 1000;
static const char* kProtocolInfo = 
"http-get:*:audio/x-flac:*,"
"http-get:*:audio/wav:*,"
"http-get:*:audio/wave:*,"
"http-get:*:audio/x-wav:*,"
"http-get:*:audio/mpeg:*,"
"http-get:*:audio/x-mpeg:*,"
"http-get:*:audio/mp1:*,"
"http-get:*:audio/aiff:*,"
"http-get:*:audio/x-aiff:*,"
"http-get:*:audio/x-m4a:*,"
"http-get:*:audio/x-ms-wma:*,"
"rtsp-rtp-udp:*:audio/x-ms-wma:*,"
"http-get:*:audio/x-scpls:*,"
"http-get:*:audio/x-mpegurl:*,"
"http-get:*:audio/x-ms-asf:*,"
"http-get:*:audio/x-ms-wax:*,"
"http-get:*:audio/x-ms-wvx:*,"
"http-get:*:text/xml:*,"
"http-get:*:audio/aac:*,"
"http-get:*:audio/aacp:*,"
"http-get:*:audio/mp4:*,"
"http-get:*:audio/ogg:*,"
"http-get:*:audio/x-ogg:*,"
"http-get:*:application/ogg:*,"
"http-get:*:video/mpeg:*,"
"http-get:*:video/mp4:*,"
"http-get:*:video/quicktime:*,"
"http-get:*:video/webm:*,"
"http-get:*:video/x-ms-wmv:*,"
"http-get:*:video/x-ms-asf:*,"
"http-get:*:video/x-msvideo:*,"
"http-get:*:video/x-ms-wax:*,"
"http-get:*:video/x-ms-wvx:*,"
"http-get:*:video/x-m4v:*,"
"http-get:*:video/x-matroska:*,"
"http-get:*:application/octet-stream:*"
;

class StandbyHandler : public IStandbyHandler
{
public: 
    virtual void SetStandby(TBool aValue);
};

void StandbyHandler::SetStandby(TBool aValue)
{
  Log::Print("Setting Standby: %d\n", aValue);
}

class SourceIndexHandler : public ISourceIndexHandler
{
public:
    virtual void SetSourceIndex(TUint aValue);
};

void SourceIndexHandler::SetSourceIndex(TUint aValue)
{
  Log::Print("Setting source: %d\n", aValue);
}

int main(int aArgc, char* aArgv[])
{
    OptionParser parser;

    OptionString optionRenderer("-r", "--renderer", Brn("dummy"), "Renderer module name.");
    parser.AddOption(&optionRenderer);

    if (!parser.Parse(aArgc, aArgv)) {
        return (1);
    }

    Net::InitialisationParams* initParams = Net::InitialisationParams::Create();

    Net::Library* lib = new Net::Library(initParams);

    Net::DvStack* dvStack = lib->StartDv();

    Brhz udn("4c494e4e-device1");

    Net::DvDeviceStandard* device = new Net::DvDeviceStandard(*dvStack, udn);

    SourceIndexHandler* sourceIndexHandler = new SourceIndexHandler();
    StandbyHandler* standbyHandler = new StandbyHandler();

    IRenderer* renderer = NULL;
    if (0 == strcmp((const char*)optionRenderer.Value().Ptr(), "dummy"))
    {
      renderer = new Dummy(lib->Env());
    }
#ifdef HAVE_VLC_VLC_H
    else
    {
      renderer = new Vlc(lib->Env());
    }
#endif

    std::vector<OpenHome::NetworkAdapter*>* subnetList = lib->CreateSubnetList();
    TIpAddress adapter = (*subnetList)[0]->Address();
    OpenHome::Net::Library::DestroySubnetList(subnetList);

    char url[1024];
    char attributes[1024];

    sprintf(url, "%d.%d.%d.%d", adapter&0xff, (adapter>>8)&0xff, (adapter>>16)&0xff, (adapter>>24)&0xff);
    Config::GetInstance().GetAbout().SetUrl(url);
    sprintf(url, "http://%s:%s/", url, kHttpPort);
    sprintf(attributes, "Info Time App:Config=%s Volume", url);
    Config::GetInstance().GetAbout().SetVersion(VERSION);

    Player* player = new Player(
        renderer,
        *device, 
        *standbyHandler, 
        *sourceIndexHandler, 
        true,
        attributes,
        "OpenHome",
        "OpenHome Consortium",
        "http://openhome.org",
        "",
        "OpenHome Media Player",
        "",
        "",
        "",
        Config::GetInstance().GetString("device", "room").c_str(),
        Config::GetInstance().GetString("device", "name").c_str(),
        "",
        url,
        "");

    SourcePlaylist* sourcePlaylist = new SourcePlaylist(*device, kTracksMax, kProtocolInfo, *player);

    player->AddSource(sourcePlaylist);

    device->SetEnabled();

    char c = '\0';
    while (c != 'q') {
        scanf("%c", &c);
    }

    Log::Print("Quiting...\n");

    delete sourcePlaylist;
    delete device;
    delete sourceIndexHandler;
    delete standbyHandler;
    delete player;

    Log::Print("Exit complete\n");
	
    return (0);
}
