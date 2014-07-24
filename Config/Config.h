#ifndef HEADER_OPENHOME_MEDIA_CONFIG
#define HEADER_OPENHOME_MEDIA_CONFIG

#include <iostream>

class CivetServer;
class JsonHandle;

extern const char * kHttpPort;

class About
{
private:
  std::string iUrl;
  std::string iMac;
  std::string iVersion;
  std::string iUpdateAvailable;
  
public:
  About()
    : iUrl("127.0.0.1")
    , iMac("00:00:00:00:00:00")
    , iVersion("0.0.1")
    , iUpdateAvailable("None")
  {};
  
  std::string& GetUrl();
  void SetUrl(const char *aValue);
  
  std::string& GetMac();
  void SetMac(const char *aValue);
  
  std::string& GetVersion();
  void SetVersion(const char *aValue);
  
  std::string& GetUpdateAvailable();
  void SetUpdateAvailable(const char *aValue);
};

class Config
{
  public:
    static Config& GetInstance()
    {
      static Config instance;
      return instance;
    }

    std::string GetString(const char *aSect, const char *aKey);
    int GetInt(const char *aSect, const char *aKey);
    bool GetBool(const char *aSect, const char *aKey);
  
    About& GetAbout();

  private:
    Config();
    Config(const Config&);
    Config& operator=(const Config&);
    ~Config();
  
    About iAbout;

    JsonHandle *iRoot;
    CivetServer *iServer;
};

#endif // HEADER_OPENHOME_MEDIA_CONFIG