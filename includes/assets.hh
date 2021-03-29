/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// assets.hh: header file for the asset holder

#ifndef M_ASSETS_HH
#define M_ASSETS_HH

#include "abase.hh"
#include "config.hh"
#include "font.hh"
#include "game/stage.hh"
#include "inherit.hh"
#include "model.hh"
#include "models.hh"
#include "module.hh"
#include "msg.hh"
#include "sbuf.hh"
#include "shape.hh"
#include "sounds.hh"

namespace hiemalia {

struct GameAssets {
    std::shared_ptr<Font> gameFont;
    std::shared_ptr<Font> menuFont;
    std::vector<sound_t> sounds;
    std::vector<std::string> musicTracks;
    std::vector<std::shared_ptr<Model>> gameModels;
    std::vector<std::shared_ptr<GameSection>> sectionData;
};

const GameAssets& getAssets();
std::shared_ptr<const Model> getGameModel(GameModel model);
const GameSection& getSectionById(size_t id);

}  // namespace hiemalia

#endif  // M_ASSETS_HH
