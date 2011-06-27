#ifndef HEADER_OPENHOME_MEDIAPLAYER_SOURCE
#define HEADER_OPENHOME_MEDIAPLAYER_SOURCE

#include <OhNetTypes.h>
#include <Buffer.h>
#include "Standard.h"

namespace OpenHome {
namespace MediaPlayer {

class Source : public Observable
{
public:
    static const TUint kMaxTypeBytes = 20;
    static const TUint kMaxNameBytes = 20;

public:
    bool Details(Bwx& aSystemName, Bwx& aType, Bwx& aName); //returns Visibility boolean
    void SetName(const Brx& aValue);
    void SetVisible(bool aValue);
public:
    virtual void Finished(uint32_t aId) = 0;
    virtual void Next(uint32_t aAfterId, uint32_t& aNextId, Bwx& aUri, Bwx& aProvider) = 0;
    virtual void Buffering(uint32_t aId) = 0;
    virtual void Playing(uint32_t aId) = 0;

protected:
    Source(const Brx& aSystemName, const Brx& aType, const Brx& aName, bool aVisible);

private:
    friend class ProviderProduct;
    void SetModifiable(ILockable& aLockable, IObserver& aObserver);
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

