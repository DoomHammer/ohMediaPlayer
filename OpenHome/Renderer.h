#ifndef HEADER_OPENHOME_MEDIAPLAYER_RENDERER
#define HEADER_OPENHOME_MEDIAPLAYER_RENDERER

namespace OpenHome {
namespace MediaPlayer {

class IRendererStatus
{
public:
    virtual void Finished(uint32_t aHandle, uint32_t aId) = 0;
    virtual void Next(uint32_t aHandle, uint32_t aAfterId, uint32_t& aId, uint8_t aUri[], uint32_t& aUriBytes) = 0;
    virtual void Buffering(uint32_t aHandle, uint32_t aId) = 0;
    virtual void Stopped(uint32_t aHandle, uint32_t aId) = 0;
    virtual void Paused(uint32_t aHandle, uint32_t aId) = 0;
    virtual void Started(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName) = 0;
    virtual void Playing(uint32_t aHandle, uint32_t aId, uint32_t aSeconds) = 0;
    virtual void Metatext(uint32_t aHandle, uint32_t aId, uint8_t aDidlLite[], uint32_t aBytes) = 0;
    virtual ~IRendererStatus() {}
};

class IRenderer
{
public:
    virtual void Play(uint32_t aHandle, uint32_t aId, const uint8_t aUri[], uint32_t aUriBytes, uint32_t aSecond) = 0;
    virtual void Pause() = 0;
    virtual void Unpause() = 0;
    virtual void Stop() = 0;
    virtual void Invalidate(uint32_t aIdFinal) = 0;
    virtual void SetStatusHandler(IRendererStatus& aHandler) = 0;
    virtual ~IRenderer() {}
};


} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_RENDERER

