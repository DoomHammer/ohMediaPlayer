#ifndef HEADER_OPENHOME_MEDIAPLAYER_RENDERER
#define HEADER_OPENHOME_MEDIAPLAYER_RENDERER

#include <string>

namespace OpenHome {
namespace MediaPlayer {

class IRenderer
{
public:
    virtual void Play(uint32_t aId, string aUri, uint32_t aSecond, string aProvider) = 0;
    virtual void Pause() = 0;
    virtual void Unpause() = 0;
    virtual void Stop() = 0;
    virtual void Invalidate(uint32_t aIdFinal) = 0;
    virtual void SetStatusHandler(IRendererStatus& aHandler) = 0;
    virtual void ~IRenderer() {}
};

class IRendererStatus
{
public:
    virtual void Finished(uint32_t aId) = 0;
    virtual void Next(uint32_t& aId, string& aUri, string& aProvider) = 0;
    virtual void Started(uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aSampleRate, bool aLossless, string aCodecName) = 0;
    virtual void Playing(uint32_t aId, uint32_t aSeconds);
    virtual void Metatext(uint32_t aId, string aDidlLite) = 0;
    virtual void ~IRendererStatus() {}
};


} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_RENDERER

