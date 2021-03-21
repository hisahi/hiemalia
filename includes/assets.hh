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

namespace hiemalia {

struct GameAssets {
    std::shared_ptr<Font> gameFont;
    std::shared_ptr<Font> menuFont;
};

const GameAssets& getAssets();

}  // namespace hiemalia

#endif  // M_AUDIO_HH
