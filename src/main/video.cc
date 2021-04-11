/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// video.cc: implementation of VideoEngine

#include "video.hh"

#include "hbase.hh"
#include "math.hh"
#include "vbase.hh"

namespace hiemalia {
void VideoConfig::load(ConfigSectionStore store) {
    fullScreen = store.get<bool>("FullScreen", fullScreen);
}

void VideoConfig::save(ConfigSectionStore store) const {
    store.set<bool>("FullScreen", fullScreen);
}

VideoEngine::VideoEngine(std::shared_ptr<HostModule> host, GameState& state)
    : video_(getVideoModule(host)),
      config_(state.config.section<VideoConfig>()) {
    readConfig();
}

void VideoEngine::gotMessage(const VideoMessage& msg) {}

void VideoEngine::frame(const SplinterBuffer& sbuf) {
    video_->frame();
    video_->blank();
    video_->draw(sbuf);
    video_->blit();
}

void VideoEngine::sync() { video_->sync(); }

void VideoEngine::readConfig() {
    bool fullScreen = config_->fullScreen;
    if (canSetFullScreen()) video_->setFullScreen(fullScreen);
}

bool VideoEngine::isFullScreen() { return video_->isFullScreen(); }

bool VideoEngine::canSetFullScreen() { return video_->canSetFullscreen(); }

Color Color::fromHSVA(float h, float s, float v, float a) {
    float h6 = h * 6.0f;
    unsigned u = floatToWholeFrac<unsigned, float>(h6);
    float p = v * (1 - s);
    float q = v * (1 - s * h6);
    float t = v * (1 - s * (1 - h6));
    float r, g, b;

    switch (u) {
        case 0:
            r = v, g = t, b = p;
            break;
        case 1:
            r = q, g = v, b = p;
            break;
        case 2:
            r = p, g = v, b = t;
            break;
        case 3:
            r = p, g = q, b = v;
            break;
        case 4:
            r = t, g = p, b = v;
            break;
        case 5:
            r = v, g = p, b = q;
            break;
        default:
            never("broken");
    }

    return Color{static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255),
                 static_cast<uint8_t>(b * 255), static_cast<uint8_t>(a * 255)};
}

}  // namespace hiemalia
