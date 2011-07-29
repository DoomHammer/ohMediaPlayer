//
//  Store.h
//  ohStore
//
//  Created by Keith Robertson on 16/07/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <OpenHome/Buffer.h>
#include <OpenHome/Private/Thread.h>
#include <map>
#include <fstream>

EXCEPTION(NoStore);

namespace OpenHome 
{

class IStore;

class StoreOption
{
friend class IStore;
public:
    StoreOption(IStore& aStore, const Brx& aKey);
    virtual ~StoreOption();
protected:
    virtual void Initialised() = 0;
    void DoSet();
    void Lock() const;
    void Unlock() const;
protected:
    Bwh iEncodedValue;
    Bwh iEncodedDefault;
    Bwh iKey;
private:
    void SetValue(const Brx& aValue);
    void SetDefault(const Brx& aDefault);
private:
    IStore& iStore;
    uint32_t iHandle;
};

class StoreOptionNum : public StoreOption
{
public:
    StoreOptionNum(IStore& aStore, const char* aKey);
    int32_t Get() const; 
    int32_t Default() const;
    void Set(int32_t aValue);
private:
    virtual void Initialised();
private:
    int32_t iValue;
    int32_t iDefault;
};

class StoreOptionString : public StoreOption
{
public:
    StoreOptionString(IStore& aStore, const char* aKey);
    void Get(Bwx& aValue) const;
    const Brx& Default() const;
    void Set(const Brx& aValue);
    //void Set(const std::string& aValue);
    //void Set(const char* aValue);
private:
    virtual void Initialised();
};

class IStore 
{
public:
    virtual ~IStore() {}
    virtual void SetAllDefaults() = 0;
    virtual void Mount() = 0;
protected:
    virtual void SetValue(StoreOption& aOption, const Brx& aValue) {aOption.SetValue(aValue);}
    virtual void SetDefault(StoreOption& aOption, const Brx& aDefault) {aOption.SetDefault(aDefault);} 
private:
    friend class StoreOption;
    virtual uint32_t Register(const Brx& aKey, StoreOption& aStoreOption) = 0;
    virtual void Set(uint32_t aHandle, const Brx& aKey, const Brx& aEncodedValue) = 0;
    virtual void Lock() const = 0;
    virtual void Unlock() const = 0;
};

class StoreFile : public IStore
{
public:
    StoreFile(const std::string& aStoreFile, const std::string& aDefaultsFile);
    ~StoreFile();
    void SetAllDefaults();
    void Mount();
private:
    uint32_t Register(const Brx& aKey, StoreOption& aStoreOption);
    void Set(uint32_t aHandle, const Brx& aKey, const Brx& aEncodedValue);
    void Lock() const;
    void Unlock() const;
private:
    uint32_t Hash(const Brx& aKey) const;
    void MountStore();
    void MountDefaults();
private:
    typedef std::pair<StoreOption*, int32_t> MapValue;
    typedef std::map<uint32_t, MapValue> Map; 
    Map iMap;
    mutable Mutex iMutex;
    std::ifstream iDefaults;
    std::fstream iStore;
    std::string iStoreFile;
};

} //namespace OpenHome
