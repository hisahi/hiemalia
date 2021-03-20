/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// test.cc: implementation of test mode

#include "test.hh"

#include <vector>

#include "load2d.hh"
#include "logger.hh"
#include "model.hh"

namespace hiemalia {

static const Color white{255, 255, 255, 255};

TestMode::TestMode() : pos_(0, 0, 1), rot_(0, 0, 0) {
    // yksinkertainen kuutiomalli
    std::vector<ModelFragment> fragments;
    using P = ModelPoint;
    fragments.push_back(
        ModelFragment(white, P(1, 1, 1),
                      {P(-1, 1, 1), P(-1, 1, -1), P(1, 1, -1), P(-1, 1, 1)}));
    fragments.push_back(ModelFragment(
        white, P(1, -1, 1),
        {P(-1, -1, 1), P(-1, -1, -1), P(1, -1, -1), P(1, -1, 1)}));
    fragments.push_back(
        ModelFragment(white, P(1, 1, 1),
                      {P(-1, 1, 1), P(-1, -1, 1), P(1, -1, 1), P(1, 1, 1)}));
    fragments.push_back(ModelFragment(
        white, P(1, 1, -1),
        {P(-1, 1, -1), P(-1, -1, -1), P(1, -1, -1), P(1, 1, -1)}));
    cube_ = Model(std::move(fragments));
    font_.setFont(std::move(loadFont("font.2d")));
    setMode(0);
}

TestMode::TestMode(TestMode&& move) noexcept
    : LogicModule(std::move(move)),
      scale_(move.scale_),
      cube_(std::move(move.cube_)),
      pos_(std::move(move.pos_)),
      rot_(std::move(move.rot_)),
      rend_(std::move(move.rend_)) {
    setMode(move.mode_);
}

TestMode& TestMode::operator=(TestMode&& move) noexcept {
    LogicModule::operator=(std::move(move));
    scale_ = move.scale_;
    cube_ = std::move(move.cube_);
    pos_ = std::move(move.pos_);
    rot_ = std::move(move.rot_);
    rend_ = std::move(move.rend_);
    setMode(move.mode_);
    return *this;
}

static const char* mode_texts[7] = {"X",     "Y",    "Z",    "YAW",
                                    "PITCH", "ROLL", "SCALE"};

void TestMode::setMode(int mode) {
    text_.clear();
    font_.renderTextLine(text_, -0.95, -0.95, white, mode_texts[mode]);
    mode_ = mode;
}

bool TestMode::run(GameState& state, float interval) {
    if (state.controls.pause) {
        setMode((mode_ + 1) % 7);
    }
    int f = state.controls.right - state.controls.left;
    if (f) {
        coord_t* p;
        switch (mode_) {
            case 0:
                p = &pos_.x;
                break;
            case 1:
                p = &pos_.y;
                break;
            case 2:
                p = &pos_.z;
                break;
            case 3:
                p = &rot_.yaw;
                break;
            case 4:
                p = &rot_.pitch;
                break;
            case 5:
                p = &rot_.roll;
                break;
            case 6:
                p = &scale_;
                break;
            default:
                never("invalid test mode");
        }
        *p += f * interval;
        LOG_DEBUG("%s = " FMT_coord_t "%s", mode_texts[mode_], *p,
                  printMatrix(rend_.getModelMatrix(pos_, rot_, scale_)));
    }
    rend_.renderModel(state.sbuf, pos_, rot_, scale_, cube_);
    state.sbuf.append(text_);
    return true;
}

}  // namespace hiemalia
