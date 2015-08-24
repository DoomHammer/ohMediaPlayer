#ifndef HEADER_OPENHOME_MEDIA_CONFIG
#define HEADER_OPENHOME_MEDIA_CONFIG

#include <iostream>
#include <unordered_map>
#include <vector>

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

class ConfigMapper
{
  protected:
    std::unordered_map<std::string, const char*> iMap;
    std::string iDefault;
  public:
    std::string& Get(const char* aKey)
    {
      auto search = iMap.find(aKey);
      std::string& ret = iDefault;

      if (search != iMap.end())
      {
        ret = search->second;
      }

      return ret;
    };

    void Append(std::string aKey, const char* aValue)
    {
      iMap[aKey] = aValue;
    };
};

class MimeTypeMapper: public ConfigMapper
{
  public:
  MimeTypeMapper()
  {
      iMap["html"] = "text/html";
      iMap["css"] = "text/css";
      iMap["js"] = "application/javascript";
      iMap["json"] = "application/json";

      iDefault = "text/plain";
  }
};

class DataMapper: public ConfigMapper
{
  public:
  DataMapper()
  {
      iMap["/data/device.json"] = "device";
      iMap["/data/volume.json"] = "volume";

      iDefault = "";
  }
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

    MimeTypeMapper& GetMimeMapper();
    DataMapper& GetDataMapper();

    std::vector<std::string>& GetControllers();
    void RegisterController(const char* aController, unsigned long long size);

    About& GetAbout();

  private:
    Config();
    Config(const Config&);
    Config& operator=(const Config&);
    ~Config();

    JsonHandle* DefaultConfig();

    About iAbout;

    MimeTypeMapper iMimeMapper;
    DataMapper iDataMapper;

    std::vector<std::string> iControllers;

    JsonHandle *iRoot;
    CivetServer *iServer;
};

#endif // HEADER_OPENHOME_MEDIA_CONFIG
