/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// assets.cc: implementation of asset store

#include "assets.hh"

#include "file.hh"
#include "game/sections.hh"
#include "game/stage.hh"
#include "load2d.hh"
#include "load3d.hh"
#include "models.hh"

namespace hiemalia {

static bool assets_loaded = false;
static GameAssets assets;

const GameAssets& getAssets() {
    if (!assets_loaded) {
        assets_loaded = true;
        assets.menuFont =
            std::make_shared<Font>(std::move(loadFont("font.2d")));
        assets.gameFont = assets.menuFont;
        assets.sectionData.insert(assets.sectionData.begin(),
                                  getSectionCount() + 1, nullptr);
        for (auto& it : sectionMap)
            assets.sectionData[static_cast<int>(it.second)] =
                std::make_shared<GameSection>(std::move(loadSection(it.first)));
    }
    return assets;
}

std::shared_ptr<const Model> getGameModel(GameModel model) {
    int index = static_cast<int>(model);
    if (index <= static_cast<int>(assets.gameModels.size()))
        assets.gameModels.insert(assets.gameModels.end(),
                                 assets.gameModels.size() - index + 1, nullptr);
    if (!assets.gameModels[index]) {
        assets.gameModels[index] = std::make_shared<Model>(
            std::move(load3D("models", modelFileNames[index])));
    }
    return assets.gameModels[index];
}

const GameSection& getSectionById(size_t id) {
    return *(getAssets().sectionData[id]);
}

void setAssetLoadedSounds(std::vector<sound_t>&& sounds) {
    assets.sounds = std::move(sounds);
}

void setAssetLoadedMusicTracks(std::vector<std::string>&& tracks) {
    assets.musicTracks = std::move(tracks);
}

}  // namespace hiemalia
