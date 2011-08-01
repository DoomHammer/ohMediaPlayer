#include <OpenHome/Store.h>
#include <OpenHome/Private/Parser.h>
#include <OpenHome/Private/Debug.h>
#include <OpenHome/Private/Ascii.h>
#include <iostream>
#include <vector>
#include <string>
#include "MurmurHash3.h"

using namespace OpenHome;
using namespace std;

StoreOption::StoreOption(IStore& aStore, const Brx& aKey)
    : iKey(aKey)
    , iStore(aStore)
{
    iHandle = iStore.Register(aKey, *this);
}

StoreOption::~StoreOption()
{
}

void StoreOption::DoSet()
{
    //Lock must be acquired by calling derived class
    iStore.Set(iHandle, iKey, iEncodedValue);
}

void StoreOption::Lock() const
{
    iStore.Lock();
}

void StoreOption::Unlock() const
{
    iStore.Unlock();
}

void StoreOption::SetValue(const Brx& aValue)
{
    //Lock held by calling Store object
    iEncodedValue.Grow(aValue.Bytes());
    iEncodedValue.Replace(aValue);
    Initialised();
}

void StoreOption::SetDefault(const Brx& aDefault)
{
    //Lock held by calling Store object
    iEncodedDefault.Grow(aDefault.Bytes());
    iEncodedDefault.Replace(aDefault);
}

StoreOptionNum::StoreOptionNum(IStore& aStore, const char* aKey)
    : StoreOption(aStore, Brn(aKey))
{
}

int32_t StoreOptionNum::Get() const
{
    int32_t val;
    Lock();
    val = iValue;
    Unlock();
    return val;
}

int32_t StoreOptionNum::Default() const
{
    //No lock required, no way to change default
    return iDefault;
}

void StoreOptionNum::Set(int32_t aValue)
{
    Lock();
    iValue = aValue;
    iEncodedValue.Grow(11); //Max size of int32_t in ascii with negative sign
    iEncodedValue.SetBytes(0);
    Ascii::AppendDec(iEncodedValue, iValue);
    DoSet();
    Unlock();
}

void StoreOptionNum::Initialised()
{
    iValue = Ascii::Int(iEncodedValue);
    iDefault = Ascii::Int(iEncodedDefault);
}

StoreOptionString::StoreOptionString(IStore& aStore, const char* aKey)
    : StoreOption(aStore, Brn(aKey))
{
}

void StoreOptionString::Get(Bwx& aValue) const
{
    Lock();
    aValue.Replace(iEncodedValue);
    Unlock();
}

const Brx& StoreOptionString::Default() const
{
    return iEncodedDefault;
}

void StoreOptionString::Set(const Brx& aValue)
{
    Lock();
    iEncodedValue.Grow(aValue.Bytes());
    iEncodedValue.Replace(aValue);
    DoSet();
    Unlock();
}

void StoreOptionString::Initialised()
{
}

StoreFile::StoreFile(const string& aStoreFile, const string& aDefaultsFile)
    : iMutex("STOR")
    , iStoreFile(aStoreFile)
{
    iDefaults.open(aDefaultsFile.c_str());
    if(iDefaults.is_open() == false) {
        iDefaults.close();
        THROW(NoStore);    
    }

    iStore.open(aStoreFile.c_str(), ios_base::in);
    if(iStore.is_open() == false) {
        iStore.open(aStoreFile.c_str(), ios_base::out|ios_base::trunc);
        while(iDefaults.good()) {
            string line;
            getline(iDefaults, line);
            if(iDefaults.eof()) {
                break;
            }
            if(line[0] != '#') {
                iStore << line;
                iStore << endl;
            }
        }
        iDefaults.close();
        iStore.close();

        iDefaults.open(aDefaultsFile.c_str());
        if(iDefaults.is_open() == false) {
            ASSERTS();
        }
        iStore.open(aStoreFile.c_str(), ios_base::in);
        if(iStore.is_open() == false) {
            ASSERTS();
        }
    }
}

StoreFile::~StoreFile()
{
    iDefaults.close();
    iStore.close();
}

uint32_t StoreFile::Register(const Brx& aKey, StoreOption& aStoreOption)
{
    uint32_t hash = Hash(aKey);
    MapValue value(&aStoreOption, -1);
    pair<uint32_t, MapValue> p(hash, value);
    pair< Map::iterator, bool> result = iMap.insert(p);

    //Can't register for the same key twice
    ASSERT(result.second == true);

    return (uint32_t)hash;
}

void StoreFile::Set(uint32_t aHandle, const Brx& aKey, const Brx& aEncodedValue)
{
    Map::iterator i = iMap.find(aHandle);
    if(i == iMap.end()) {
        ASSERTS();
    }

    int32_t oldPosition = i->second.second;

    i->second.second = iStore.tellp();

    iStore.write((const char*)aKey.Ptr(), aKey.Bytes());
    iStore << ':';
    iStore.write((const char*)aEncodedValue.Ptr(), aEncodedValue.Bytes());
    iStore << endl;

    iStore.seekp(oldPosition);
    iStore.put('!');
    iStore.seekp(0, ios_base::end);
    iStore.flush();
}

void StoreFile::SetAllDefaults()
{
    ASSERTS();
}

void StoreFile::Lock() const
{
    iMutex.Wait();
}

void StoreFile::Unlock() const
{
    iMutex.Signal();
}

void StoreFile::Mount()
{
    MountDefaults();
    MountStore();
}

uint32_t StoreFile::Hash(const Brx& aKey) const
{
    uint32_t hash;
    MurmurHash3_x86_32(aKey.Ptr(), aKey.Bytes(), 0x486f6d65, &hash);
    return hash;
}

void StoreFile::MountDefaults()
{
    string line;
    Parser parser;

    while(true) {
        ASSERT(iDefaults.good());

        getline(iDefaults, line);
        if(iDefaults.eof()) {
            break;
        }

        if(line[0] == '#') {
            continue;
        }

        Brn tmp((TByte*)line.data(), line.size());
        parser.Set(tmp);

        Brn key(parser.Next(':'));
        Brn value(parser.NextToEnd());

        uint32_t hash = Hash(key);

        MapValue mapValue((StoreOption*)0, -1);
        pair<uint32_t, MapValue> p(hash, mapValue);
        pair<Map::iterator, bool> result = iMap.insert(p);
        if(result.second == false) {
            MapValue existing = result.first->second;
            ASSERT(existing.second == -1);
            SetDefault(*(existing.first), value);
        }
    }
}

void StoreFile::MountStore()
{
    vector<string> v;

    while(true) {
        ASSERT(iStore.good());
        string line;
        getline(iStore, line);
        if(iStore.eof()) {
            break;
        }
        v.push_back(line);    
    }

    iStore.close();
    iStore.open(iStoreFile.c_str(), ios_base::out|ios_base::trunc);

    Parser parser;

    vector<string>::iterator i;
    for( i = v.begin(); i != v.end(); i++) {
        //If obsolete entry, don't rewrite
        if((*i)[0] == '!') {
            continue;
        }

        int32_t pos = iStore.tellp();
        iStore << *i;
        iStore << endl;

        Brn tmp((const TByte*)i->data(), i->size());
        parser.Set(tmp);
        Brn key(parser.Next(':'));
        Brn value(parser.NextToEnd());

        uint32_t hash = Hash(key);
        Map::iterator j = iMap.find(hash);
        ASSERT(j != iMap.end());

        j->second.second = pos;
        SetValue(*(j->second.first), value);
    }

    iStore.flush();
}

