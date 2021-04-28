/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// arcadeoverlay.cc: implementation of the menu arcade overlay

#include "arcadeoverlay.hh"

#include "assets.hh"
#include "audio.hh"
#include "hiemalia.hh"

namespace hiemalia {

ArcadeOverlay::ArcadeOverlay(std::shared_ptr<ModuleHolder> holder)
    : holder_(holder) {
    font_.setFont(getAssets().menuFont);
}

ArcadeOverlay::~ArcadeOverlay() noexcept {}

bool ArcadeOverlay::run(GameState& state, float interval) {
    if (visible_ && (haveCredits_ || blink_ < 0.75)) state.sbuf.append(buf_);
    if (visible_ && haveCredits_ && state.arcade && state.controls.fire) {
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
        sendMessage(LogicMessage::startGameArcade(holder_));
    }
    blink_ = frac(blink_ + interval);
    return true;
}

void ArcadeOverlay::visible(bool visible) { visible_ = visible; }

void ArcadeOverlay::redraw(int credits, bool freePlay) {
    buf_.clear();
    haveCredits_ = freePlay || credits > 0;
    if (haveCredits_) {
        font_.drawTextLineLeft(buf_, -0.875, 0.875, Color{64, 255, 64, 192},
                               "PRESS FIRE", 0.75);
        font_.drawTextLineLeft(buf_, -0.875, 0.9375, Color{64, 255, 64, 192},
                               "TO BEGIN", 0.75);
    }
    if (freePlay) {
        font_.drawTextLineRight(buf_, 0.875, 0.875, Color{64, 255, 255, 255},
                                "FREE", 0.75);
        font_.drawTextLineRight(buf_, 0.875, 0.9375, Color{64, 255, 255, 255},
                                "PLAY", 0.75);
    } else if (credits == 0) {
        font_.drawTextLineRight(buf_, 0.875, 0.875, Color{255, 255, 0, 255},
                                "INSERT", 0.75);
        font_.drawTextLineRight(buf_, 0.875, 0.9375, Color{255, 255, 0, 255},
                                "COIN", 0.75);
    } else if (credits > 0) {
        font_.drawTextLineRight(buf_, 0.875, 0.875, Color{255, 255, 0, 255},
                                "CREDIT", 0.75);
        font_.drawTextLineRight(buf_, 0.875, 0.9375, Color{255, 255, 0, 255},
                                std::to_string(credits), 0.75);
    }
}

}  // namespace hiemalia
