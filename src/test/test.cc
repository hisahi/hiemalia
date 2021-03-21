/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// test.cc: implementation of test mode

#include "test.hh"

#include <vector>

#include "hiemalia.hh"
#include "load2d.hh"
#include "load3d.hh"
#include "logger.hh"
#include "model.hh"

namespace hiemalia {

static const Color white{255, 255, 255, 255};

TestMode::TestMode() : pos_(0, 0, 0.5), rot_(0, 0, 0), scale_(1.0) {
    cube_ = load3D("ship.3d");
    font_.setFont(std::move(loadFont("font.2d")));
    setMode(0);
    rend_.setCamera(ModelPoint(0, -0.5, 0), Rotation3D(0, -0.5, 0));
}

TestMode::TestMode(TestMode&& move) noexcept
    : LogicModule(std::move(move)),
      cube_(std::move(move.cube_)),
      pos_(move.pos_),
      rot_(move.rot_),
      scale_(move.scale_),
      rend_(std::move(move.rend_)) {
    setMode(move.mode_);
}

TestMode& TestMode::operator=(TestMode&& move) noexcept {
    LogicModule::operator=(std::move(move));
    cube_ = std::move(move.cube_);
    pos_ = move.pos_;
    rot_ = move.rot_;
    scale_ = move.scale_;
    rend_ = std::move(move.rend_);
    setMode(move.mode_);
    return *this;
}

static const char* mode_texts[2] = {"XYZ", "ROT"};

void TestMode::setMode(int mode) {
    text_.clear();
    font_.renderTextLine(text_, -0.95, -0.95, white, mode_texts[mode]);
    mode_ = mode;
}

void TestMode::gotMessage(const MenuMessage& msg) {
    if (msg.type == MenuMessageType::MenuExit) sendMessage(GameMessage::quit());
}

bool TestMode::run(GameState& state, float interval) {
    if (state.controls.pause) {
        setMode((mode_ + 1) % 2);
    }
    int x = state.controls.right - state.controls.left;
    int y = state.controls.down - state.controls.up;
    int z = state.controls.forward - state.controls.back;

    if (x != 0) {
        coord_t* p;
        const char* s;
        switch (mode_) {
            case 0:
                p = &pos_.x;
                s = "X";
                break;
            case 1:
                p = &rot_.yaw;
                s = "YAW";
                break;
            default:
                never("invalid test mode");
        }
        *p += x * interval;

        LOG_DEBUG("%s = " FMT_coord_t, s, *p);
    }
    if (y != 0) {
        coord_t* p;
        const char* s;
        switch (mode_) {
            case 0:
                p = &pos_.y;
                s = "Y";
                break;
            case 1:
                p = &rot_.pitch;
                s = "PITCH";
                break;
            default:
                never("invalid test mode");
        }
        *p += y * interval;

        LOG_DEBUG("%s = " FMT_coord_t, s, *p);
    }
    if (z != 0) {
        coord_t* p;
        const char* s;
        switch (mode_) {
            case 0:
                p = &pos_.z;
                s = "Z";
                break;
            case 1:
                p = &rot_.roll;
                s = "ROLL";
                break;
            default:
                never("invalid test mode");
        }
        *p += z * interval;

        LOG_DEBUG("%s = " FMT_coord_t, s, *p);
    }

    rend_.renderModel(state.sbuf, pos_, rot_, scale_, cube_);
    state.sbuf.append(text_);
    return true;
}

}  // namespace hiemalia
