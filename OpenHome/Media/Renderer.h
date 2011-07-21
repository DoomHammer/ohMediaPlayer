#ifndef HEADER_OPENHOME_MEDIA_RENDERER
#define HEADER_OPENHOME_MEDIA_RENDERER

#include <ohNetTypes.h>

namespace OpenHome {
namespace Media {

class ITrack;

class IRendererStatus
{
public:
    virtual void Finished(uint32_t aHandle, uint32_t aId) = 0;
    virtual const class ITrack* Next(uint32_t aHandle, uint32_t aAfterId) = 0;
    virtual void Buffering(uint32_t aHandle, uint32_t aId) = 0;
    virtual void Playing(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName) = 0;
    virtual void Time(uint32_t aHandle, uint32_t aId, uint32_t aSeconds) = 0;
    virtual void Metatext(uint32_t aHandle, uint32_t aId, uint8_t aDidlLite[], uint32_t aBytes) = 0;
    virtual ~IRendererStatus() {}
};

class IRenderer
{
public:
    virtual void Play(uint32_t aHandle, const class ITrack& aTrack, uint32_t aSecond) = 0;
    virtual void Pause() = 0;
    virtual void Unpause() = 0;
    virtual void Stop() = 0;
    virtual void FinishAfter(uint32_t aId) = 0;
    virtual void SetVolume(uint32_t aValue) = 0;
    virtual void SetMute(bool aValue) = 0;
    virtual void SetStatusHandler(IRendererStatus& aHandler) = 0;
    virtual ~IRenderer() {}
};


} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_RENDERER

