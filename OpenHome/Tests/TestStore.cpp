#include <OpenHome/Store.h>
#include <OpenHome/Private/TestFramework.h>
#include <OpenHome/Private/Debug.h>

#ifdef _WIN32
#define SHARE "install\\share\\Tests\\TestStore\\"
#else 
#define SHARE "install/share/Tests/TestStore/"
#endif

using namespace OpenHome;
using namespace OpenHome::TestFramework;

class SuiteStoreNumbers : public Suite
{
public:
    SuiteStoreNumbers() : Suite("Numbers") {}
    void Test();
};

void SuiteStoreNumbers::Test()
{    
    {
    StoreFile store(SHARE"store0.txt", SHARE"defaults0.txt");

    StoreOptionNum volume(store, "OpenHome.Volume");
    StoreOptionNum volumeLimit(store, "OpenHome.Volume.Limit");

    store.Mount();

    TEST(volume.Default() == 50);
    TEST(volumeLimit.Default() == 80);

    volume.Set(volume.Default());
    volumeLimit.Set(volumeLimit.Default());
    TEST(volume.Get() == 50);
    TEST(volumeLimit.Get() == 80);

    volume.Set(51);
    TEST(volume.Get() == 51);
    volumeLimit.Set(100);
    TEST(volumeLimit.Get() == 100);

    volume.Set(volume.Default());
    volumeLimit.Set(volumeLimit.Default());
    TEST(volume.Get() == 50);
    TEST(volumeLimit.Get() == 80);

    volume.Set(0);
    TEST(volume.Get() == 0);

    volumeLimit.Set(-1);
    TEST(volumeLimit.Get() == -1);

    volume.Set(INT32_MAX); //2^31
    TEST(volume.Get() == INT32_MAX);

    volumeLimit.Set(INT32_MIN); //-2^31-1
    TEST(volumeLimit.Get() == INT32_MIN);

    volume.Set(75);
    volumeLimit.Set(90);

    TEST(volume.Get() == 75);
    TEST(volumeLimit.Get() == 90);
    }

    {
    StoreFile store(SHARE"store0.txt", SHARE"defaults0.txt");

    StoreOptionNum volume(store, "OpenHome.Volume");
    StoreOptionNum volumeLimit(store, "OpenHome.Volume.Limit");

    store.Mount();

    TEST(volume.Get() == 75);
    TEST(volumeLimit.Get() == 90);

    volume.Set(51);
    TEST(volume.Get() == 51);
    volumeLimit.Set(100);
    TEST(volumeLimit.Get() == 100);

    TEST(volume.Default() == 50);
    TEST(volumeLimit.Default() == 80);

    volume.Set(volume.Default());
    volumeLimit.Set(volumeLimit.Default());
    TEST(volume.Get() == 50);
    TEST(volumeLimit.Get() == 80);

    volume.Set(INT32_MAX); //2^31
    TEST(volume.Get() == INT32_MAX);

    volumeLimit.Set(INT32_MIN); //-2^31
    TEST(volumeLimit.Get() == INT32_MIN);
    }

    TEST_THROWS(StoreFile(SHARE"store.txt", "ThisFileDoesntExist.txt"), NoStore);
}

class SuiteStoreStrings : public Suite
{
public:
    SuiteStoreStrings() : Suite("Strings") {}
    void Test();
};

void SuiteStoreStrings::Test()
{    
    StoreFile store(SHARE"storeString0.txt", SHARE"defaultsString0.txt");
    
    StoreOptionString room(store, "OpenHome.Room");
    StoreOptionString name(store, "OpenHome.Name");

    store.Mount();

    TEST(room.Default() == Brn("Living Room"));
    TEST(name.Default() == Brn("My Media Renderer"));

    const Brn kitchen("Kitchen");
    room.Set(kitchen);
    Bws<20> result;
    room.Get(result);
    TEST(result == kitchen);

    const Brn name2("Bird is the word");
    name.Set(name2);
    name.Get(result);
    TEST(result == name2);
}

void OpenHome::TestFramework::Runner::Main(TInt /*aArgc*/, TChar* /*aArgv*/[], Net::InitialisationParams* aInitParams)
{
    Net::UpnpLibrary::InitialiseMinimal(aInitParams);

    Runner runner("Store Tests");
    runner.Add(new SuiteStoreNumbers());
    runner.Add(new SuiteStoreStrings());
    runner.Run();
}
