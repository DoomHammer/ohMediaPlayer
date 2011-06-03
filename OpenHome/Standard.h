#ifndef HEADER_OPENHOME_MEDIAPLAYER_STANDARD
#define HEADER_OPENHOME_MEDIAPLAYER_STANDARD

namespace OpenHome {
namespace MediaPlayer {

class ILockable
{
public:
    virtual void Wait() = 0;
    virtual void Signal() = 0;
    virtual ~ILockable() {}
};

} //namespace MediaPlayer
} //namespace OpenHome

#endif HEADER_OPENHOME_MEDIAPLAYER_STANDARD
