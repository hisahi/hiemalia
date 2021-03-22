/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// assets.cc: implementation of asset store

#include "assets.hh"

#include "load2d.hh"
#include "load3d.hh"

namespace hiemalia {
static bool assets_loaded = false;
static GameAssets assets;

const GameAssets& getAssets() {
    if (!assets_loaded) {
        assets_loaded = true;
        assets.menuFont =
            std::make_shared<Font>(std::move(loadFont("font.2d")));
        assets.gameFont = assets.menuFont;
    }
    return assets;
}

void setAssetLoadedSounds(std::vector<sound_t>&& sounds) {
    assets.sounds = std::move(sounds);
}

void setAssetLoadedMusicTracks(std::vector<std::string>&& tracks) {
    assets.musicTracks = std::move(tracks);
}

}  // namespace hiemalia
