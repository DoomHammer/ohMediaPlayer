#ifndef HEADER_OPENHOME_MEDIA_STANDARD
#define HEADER_OPENHOME_MEDIA_STANDARD

#include <vector>

namespace OpenHome {
namespace Media {

class ILockable
{
public:
    virtual void Wait() const = 0;
    virtual void Signal() const = 0;
    virtual ~ILockable() {}
};

class IObserver
{
public:
    virtual void ObservableChanged() = 0;
    virtual ~IObserver() {}
};

class Observable
{
public:
    Observable();
    void RegisterObserver(IObserver& aObserver);

protected:
    void InformObservers() const;

private:
    std::vector<IObserver*> iObserverList;
};

} // namespace Media
} // namespace OpenHome

#endif //HEADER_OPENHOME_MEDIA_STANDARD
