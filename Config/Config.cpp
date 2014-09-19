#include <Config/Config.h>
#include <Config/resources.h>

#include <OpenHome/Private/Debug.h>

#include <CivetServer.h>
#include <JsonHandle.h>

#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

const char* kHttpPort = "8080";
const char* kConfigFile = "config.json";
const char* kDefaultConfig = R"delimiter(
{
 "device": {
  "room": "OpenHomeKeith",
  "name": "ohMediaPlayer"
 },
 "volume": {
  "startupVolume": "40",
  "volumeLimit": "80"
 }
}
)delimiter";

void FileNotFoundHandler(struct mg_connection* aConn)
{
  mg_printf(aConn, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
}

// via http://techoverflow.net/blog/2013/01/11/cpp-check-if-file-exists/
bool fexists(const char* filename)
{
  ifstream ifile(filename);
  return ifile;
}

void OkHandler(struct mg_connection* aConn, const char* aContentType)
{
  mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", aContentType);
}

string& MimeFromUri(string& aUri)
{
  size_t rfind = aUri.rfind(".");
  string ext(rfind == string::npos ? "" : aUri.substr(rfind+1, string::npos));
  return Config::GetInstance().GetMimeMapper().Get(ext.c_str());
}

class CommonHandler: public CivetHandler
{
  protected:
    bool handleAll(const char *aMethod, CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      string uri = request_info->uri;

      OpenHome::Log::Print("%s: %s\n", aMethod, uri.c_str());

      string contentType("text/html"), content;

      if (internalHandler(aMethod, aConn, uri, contentType, content))
      {
        OkHandler(aConn, contentType.c_str());
        if (content.length())
        {
          mg_printf(aConn, "%s", content.c_str());
        }
      }
      else
      {
        FileNotFoundHandler(aConn);
      }

      return true;
    }

    virtual bool internalHandler(const char *aMethod, struct mg_connection *aConn, string& aUri, string& aContentType, string& aContent) = 0;

  public:
    bool handleGet(CivetServer *aServer, struct mg_connection *aConn)
    {
      return handleAll("GET", aServer, aConn);
    }

    bool handlePost(CivetServer *aServer, struct mg_connection *aConn)
    {
      return handleAll("POST", aServer, aConn);
    }
};

class StaticHandler: public CommonHandler
{
  private:
    virtual bool internalHandler(const char *aMethod, struct mg_connection *aConn, string& aUri, string& aContentType, string& aContent)
    {
      bool ret = false;
      string uri(aUri);

      OpenHome::Log::Print("StaticHandler\n");

      if (uri == "/")
      {
        uri = "/index.html";
      }

      string& mime(MimeFromUri(uri));

      OpenHome::Log::Print("Content-Type: %s\n", mime.c_str());

      unsigned long long resSize;
      const char* resource = getResource(uri.c_str(), &resSize);

      if (resSize)
      {
        aContentType = mime;
        aContent = resource;
        ret = true;
      }

      return ret;
    }
};

class DataHandler: public CommonHandler
{
  private:
    JsonHandle root;

    virtual bool internalHandler(const char *aMethod, struct mg_connection *aConn, string& aUri, string& aContentType, string& aContent)
    {
      bool ret = false;
      string& field = Config::GetInstance().GetDataMapper().Get(aUri.c_str());

      OpenHome::Log::Print("DataHandler\n");

      string& mime(MimeFromUri(aUri));

      OpenHome::Log::Print("Content-Type: %s\n", mime.c_str());

      if (strcmp(aMethod, "GET") == 0)
      {
        if (field.length() > 0)
        {
          root[field].toCompactString(aContent);
          ret = true;
        }
      }
      else if (strcmp(aMethod, "POST") == 0)
      {
        char postData[1024] = "\0";
        mg_read(aConn, postData, sizeof(postData));

        JsonHandle post;
        post.fromString(postData);

        OpenHome::Log::Print("POST DATA: ##%s##\n", postData);

        if (field.length() > 0)
        {
          root[field] = post;
          root.toFile(kConfigFile);
        }
        ret = true;
      }

      return ret;
    }
  public:
    DataHandler(JsonHandle &aRoot) : root(aRoot) {};
};

class ControllersHandler: public CivetHandler
{
  private:
    vector<const char*>& iControllers;

    bool handleAll(const char *aMethod, CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      const char *resource;
      unsigned long long resSize;
      string uri = request_info->uri;

      size_t rfind = uri.rfind(".");
      string ext(rfind == string::npos ? "none" : uri.substr(rfind+1, string::npos));
      string& mime = Config::GetInstance().GetMimeMapper().Get(ext.c_str());

      OpenHome::Log::Print("Path: %s\n", uri.c_str());
      OpenHome::Log::Print("Content-Type: %s\n", mime.c_str());

      resource = getResource(uri.c_str(), &resSize);
      mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", mime.c_str());
      mg_printf(aConn, "%s", resource);

      for (auto &controller: iControllers)
      {
        mg_printf(aConn, "%s", controller);
      }

      return true;
    }

  public:
    ControllersHandler(vector<const char*>& aControllers)
      : iControllers(aControllers)
    {};

    bool handleGet(CivetServer *aServer, struct mg_connection *aConn)
    {
      return handleAll("GET", aServer, aConn);
    }

    bool handlePost(CivetServer *aServer, struct mg_connection *aConn)
    {
      return handleAll("POST", aServer, aConn);
    }
};

class AboutHandler: public CivetHandler
{
  private:
    About& iAbout;
  public:
    AboutHandler(About& aAbout) :
      iAbout(aAbout)
    {};
  
    bool handleGet(CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      string s;

      OpenHome::Log::Print("GET %s\n", request_info->uri);
      
      JsonHandle root;
      
      root["modelName"] = "ohMediaPlayer";
      root["manufacturer"] = "Linn";
      root["softwareVersion"] = iAbout.GetVersion().c_str();
      root["softwareUpdateAvailable"] = iAbout.GetUpdateAvailable().c_str();
      root["ipAddress"] = iAbout.GetUrl().c_str();
      root["macAddress"] = iAbout.GetMac().c_str();
      
      root.toCompactString(s);
      
      mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
      mg_printf(aConn, "%s", s.c_str());
      return true;
    }

    bool handlePost(CivetServer *aServer, struct mg_connection *aConn)
    {
        FileNotFoundHandler(aConn);
        return true;
    }
};

Config::Config()
{
  const char * options[] = { "document_root", "/dev/null",
    "listening_ports", kHttpPort, 0
  };

  if (fexists(kConfigFile))
  {
    iRoot = new JsonHandle();
    iRoot->fromFile(kConfigFile);
  }
  else
  {
    iRoot = DefaultConfig();
  }

  iServer = new CivetServer(options);
  iServer->addHandler("/css", new StaticHandler());
  iServer->addHandler("/img", new StaticHandler());
  iServer->addHandler("/js", new StaticHandler());
  iServer->addHandler("/js/controllers.js", new ControllersHandler(GetControllers()));
  iServer->addHandler("/partials", new StaticHandler());

  iServer->addHandler("/data", new DataHandler(*iRoot));
  
  iServer->addHandler("/data/about.json", new AboutHandler(GetAbout()));

  iServer->addHandler("/", new StaticHandler());
}

Config::~Config()
{
  delete iRoot;
  delete iServer;
}

string Config::GetString(const char *aSect, const char *aKey)
{
  return (*iRoot)[aSect][aKey];
}

int Config::GetInt(const char *aSect, const char *aKey)
{
  return atoi((*iRoot)[aSect][aKey]);
}

bool Config::GetBool(const char *aSect, const char *aKey)
{
  return (*iRoot)[aSect][aKey];
}

JsonHandle* Config::DefaultConfig()
{
  JsonHandle* ret = new JsonHandle;
  ret->fromString(kDefaultConfig);
  return ret;
}

MimeTypeMapper& Config::GetMimeMapper()
{
  return iMimeMapper;
}

DataMapper& Config::GetDataMapper()
{
  return iDataMapper;
}

vector<const char *>& Config::GetControllers()
{
  return iControllers;
}

void Config::RegisterController(const char* aController)
{
  iControllers.push_back(aController);
}

About& Config::GetAbout()
{
  return iAbout;
}

string& About::GetUrl()
{
  return iUrl;
}

void About::SetUrl(const char *aValue)
{
  iUrl = aValue;
}

string& About::GetMac()
{
  return iMac;
}

void About::SetMac(const char *aValue)
{
  iMac = aValue;
}

string& About::GetVersion()
{
  return iVersion;
}

void About::SetVersion(const char *aValue)
{
  iVersion = aValue;
}

string& About::GetUpdateAvailable()
{
  return iUpdateAvailable;
}

void About::SetUpdateAvailable(const char *aValue)
{
  iUpdateAvailable = aValue;
}
