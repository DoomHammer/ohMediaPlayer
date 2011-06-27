#ifndef HEADER_OPENHOME_MEDIAPLAYER_SOURCE
#define HEADER_OPENHOME_MEDIAPLAYER_SOURCE

#include <OhNetTypes.h>
#include <Buffer.h>
#include "Standard.h"

namespace OpenHome {
namespace MediaPlayer {

class Source : public Observable
{
friend class ProviderProduct;
private:
    static const TUint kMaxTypeBytes = 20;
    static const TUint kMaxNameBytes = 20;

public:
    void SetModifiable(ILockable& aLockable, IObserver& aObserver);
    bool Details(Bwx& aSystemName, Bwx& aType, Bwx& aName); //returns Visibility boolean
    void SetName(const Brx& aValue);
    void SetVisible(bool aValue);

protected:
    Source(const Brx& aSystemName, const Brx& aType, const Brx& aName, bool aVisible);

private:
    Bws<kMaxNameBytes> iSystemName;
    Bws<kMaxTypeBytes> iType;
    Bws<kMaxNameBytes> iName;
    bool iVisible;
    ILockable* iLockable;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_SOURCE

