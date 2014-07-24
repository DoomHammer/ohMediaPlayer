#include <Config/Config.h>
#include <Config/resources.h>

#include <OpenHome/Private/Debug.h>

#include <CivetServer.h>
#include <JsonHandle.h>

#include <stdlib.h>
#include <string.h>

#include <unordered_map>

const char * kHttpPort = "8080";

class SMimeType
{
  private:
    std::unordered_map<std::string, std::string> e2m;
  public:
    SMimeType()
    {
      e2m["none"] = "text/plain";
      e2m["html"] = "text/html";
      e2m["css"] = "text/css";
      e2m["js"] = "application/javascript";
      e2m["json"] = "application/json";
    };

    const char * Get(std::string &aExt)
    {
      static const char * plain = "text/plain";
      const char * ret;
      auto search = e2m.find(aExt);

      if (search != e2m.end())
      {
        ret = search->second.c_str();
      }
      else
      {
        ret = plain;
      }

      return ret;
    }
};

SMimeType MimeType()
{
  static SMimeType m;
  return m;
}

class SFileMapper
{
  private:
    std::unordered_map<std::string, std::string> p2p;
  public:
    SFileMapper()
    {
      p2p["/data/device.json"] = "device";
      p2p["/data/volume.json"] = "volume";
    }

    const char * Get(const char *aExt)
    {
      static const char * none = "";
      const char * ret;
      auto search = p2p.find(aExt);

      if (search != p2p.end())
      {
        ret = search->second.c_str();
      }
      else
      {
        ret = none;
      }

      return ret;
    }
};

SFileMapper FileMapper()
{
  static SFileMapper m;
  return m;
}

void handle_404(struct mg_connection *aConn)
{
  mg_printf(aConn, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
}

class StaticHandler: public CivetHandler
{
  private:
    bool handleAll(const char *aMethod, CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      const char *resource;
      unsigned long long resSize;
      std::string uri = request_info->uri;

      OpenHome::Log::Print("Path: %s\n", uri.c_str());

      if (uri == "/")
      {
        uri = "/index.html";
      }

      size_t rfind = uri.rfind(".");
      std::string ext(rfind == std::string::npos ? "none" : uri.substr(rfind+1, std::string::npos));

      resource = getResource(uri.c_str(), &resSize);

      if (resSize)
      {
        mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", MimeType().Get(ext));
        mg_printf(aConn, "%s", resource);
      }
      else
      {
        handle_404(aConn);
      }

      return true;
    }

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

class DataHandler: public CivetHandler
{
  private:
    JsonHandle root;
  public:
    DataHandler(JsonHandle &aRoot) : root(aRoot) {};

    bool handleGet(CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      std::string s;
      std::string field(FileMapper().Get(request_info->uri));

      OpenHome::Log::Print("GET %s\n", request_info->uri);
      
      if (field.length() > 0)
      {
        root[field].toCompactString(s);
      }
      else
      {
        handle_404(aConn);
        return true;
      }

      mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
      mg_printf(aConn, "%s", s.c_str());
      return true;
    }

    bool handlePost(CivetServer *aServer, struct mg_connection *aConn)
    {
      const struct mg_request_info *request_info = mg_get_request_info(aConn);
      char post_data[1024] = "\0";
      mg_read(aConn, post_data, sizeof(post_data));
      JsonHandle post;
      post.fromString(post_data);
      std::string field(FileMapper().Get(request_info->uri));

      OpenHome::Log::Print("POST %s\n", request_info->uri);
      OpenHome::Log::Print("POST DATA: ##%s##\n", post_data);
      
      if (field.length() > 0)
      {
        root[field] = post;
      }
      else
      {
        handle_404(aConn);
        return true;
      }

      root.toFile("config.json");

      mg_printf(aConn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
      return true;
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
      std::string s;

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
        handle_404(aConn);
        return true;
    }
};

Config::Config()
{
  const char * options[] = { "document_root", "/dev/null",
    "listening_ports", kHttpPort, 0
  };

  iRoot = new JsonHandle();
  iRoot->fromFile("config.json");

  iServer = new CivetServer(options);
  iServer->addHandler("/css", new StaticHandler());
  iServer->addHandler("/img", new StaticHandler());
  iServer->addHandler("/js", new StaticHandler());
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

std::string Config::GetString(const char *aSect, const char *aKey)
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

About& Config::GetAbout()
{
  return iAbout;
}

std::string& About::GetUrl()
{
  return iUrl;
}

void About::SetUrl(const char *aValue)
{
  iUrl = aValue;
}

std::string& About::GetMac()
{
  return iMac;
}

void About::SetMac(const char *aValue)
{
  iMac = aValue;
}

std::string& About::GetVersion()
{
  return iVersion;
}

void About::SetVersion(const char *aValue)
{
  iVersion = aValue;
}

std::string& About::GetUpdateAvailable()
{
  return iUpdateAvailable;
}

void About::SetUpdateAvailable(const char *aValue)
{
  iUpdateAvailable = aValue;
}
