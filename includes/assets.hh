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
#include "inherit.hh"
#include "model.hh"
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
};

inline auto soundEffectNames = hiemalia::to_array<NamePair<SoundEffect>>({
    {"mselect.wav", SoundEffect::MenuSelect},
    {"mmove.wav", SoundEffect::MenuChange},
});

const GameAssets& getAssets();

}  // namespace hiemalia

#endif  // M_ASSETS_HH
