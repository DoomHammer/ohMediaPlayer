#ifndef HEADER_OPENHOME_MEDIAPLAYER_STANDARD
#define HEADER_OPENHOME_MEDIAPLAYER_STANDARD

#include <vector>

namespace OpenHome {
namespace MediaPlayer {

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

} // namespace MediaPlayer
} // namespace OpenHome

#endif HEADER_OPENHOME_MEDIAPLAYER_STANDARD
