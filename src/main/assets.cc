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
        assets.menuFont = std::make_shared<Font>(loadFont("font.2d"));
        assets.gameFont = assets.menuFont;
        assets.sectionData.insert(assets.sectionData.begin(),
                                  getSectionCount() + 1, nullptr);
        for (auto& it : sectionMap)
            assets.sectionData[static_cast<int>(it.second)] =
                std::make_shared<GameSection>(loadSection(it.first));
    }
    return assets;
}

static LoadedGameModel loadGameModel(int index) {
    ModelWithCollision mc =
        load3DWithCollision("models", modelFileNames[index]);
    return LoadedGameModel{
        std::make_shared<Model>(std::move(mc.model)),
        std::make_shared<ModelCollision>(std::move(mc.collision)), mc.radius};
}

const LoadedGameModel& getGameModel(GameModel model) {
    int index = static_cast<int>(model);
    if (index >= static_cast<int>(assets.gameModels.size()))
        assets.gameModels.insert(assets.gameModels.end(),
                                 index - assets.gameModels.size() + 1, {});
    if (!assets.gameModels[index].model) {
        assets.gameModels[index] = loadGameModel(index);
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
