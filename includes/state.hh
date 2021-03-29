/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// state.hh: header file for the game state structure

#ifndef M_STATE_HH
#define M_STATE_HH

#include <sstream>
#include <stdexcept>
#include <string>

#include "config.hh"
#include "controls.hh"
#include "sbuf.hh"

namespace hiemalia {

struct GameState {
    ControlState controls;
    Config config;
    SplinterBuffer sbuf;
};

};  // namespace hiemalia

#endif  // M_STATE_HH
