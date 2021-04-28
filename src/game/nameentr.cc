/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/nameentr.cc: implementation of the name entry screen

#include "game/nameentr.hh"

#include "assets.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
static std::uniform_real_distribution<coord_t> rd_firework_x(-1.0, 1.0);
static std::uniform_real_distribution<coord_t> rd_firework_vx(-0.125, 0.125);
static std::uniform_real_distribution<coord_t> rd_firework_vy(-1.8, -0.6);
static std::uniform_real_distribution<float> rd_firework_clr(0.0f, 1.0f);
static std::uniform_real_distribution<float> rd_firework_explodeSize(0.7f,
                                                                     1.4f);
static std::uniform_int_distribution<int> rd_firework_shards{40, 160};
static std::uniform_real_distribution<float> rd_firework_angle(
    -numbers::PI<float>, numbers::PI<float>);
static std::uniform_real_distribution<float> rd_firework_speed(0.1f, 0.5f);
static std::uniform_real_distribution<coord_t> rd_firework_rvx(-8.0, 8.0);
static std::uniform_real_distribution<coord_t> rd_firework_rvy(-8.0, 8.0);
static std::uniform_real_distribution<float> rd_firework_spawn{0, 1};
static Color rocketColor{255, 128, 128, 160};
static constexpr float rocketLength = 1.0f / 128;
static constexpr float shardLength = 1.0f / 512;

Firework::Firework()
    : pos(random(rd_firework_x), 1),
      vel(random(rd_firework_vx), random(rd_firework_vy)),
      explodeColor_(Color::fromHSVA(
          random(rd_firework_clr),
          1 - random(rd_firework_clr) * random(rd_firework_clr), 1.0f, 1.0f)) {}

bool Firework::update(SplinterBuffer& sbuf, Renderer2D& rend, float interval) {
    return explode_ ? updateExplode(sbuf, rend, interval)
                    : updateRocket(sbuf, rend, interval);
}

void Firework::explode() {
    explode_ = true;
    explodeSize_ = random(rd_firework_explodeSize);
    explodeFade_ = 1.5f / explodeSize_;
    int j = random(rd_firework_shards);
    for (int i = 0; i < j; ++i) {
        float angle = random(rd_firework_angle);
        float speed = random(rd_firework_speed) * random(rd_firework_speed) *
                      explodeSize_;
        explodePieces_.push_back(
            {Point2D(0, 0),
             Point2D(random(rd_firework_angle), random(rd_firework_angle)),
             Point2D(speed * sin(angle), speed * cos(angle)),
             Point2D(random(rd_firework_rvx), random(rd_firework_rvy))});
    }
    vel.y = 0.0f;
    vel.x *= 0.25f;
}

bool Firework::updateRocket(SplinterBuffer& sbuf, Renderer2D& rend,
                            float interval) {
    pos += vel * interval;
    Point2D rv = vel;
    vel.y += interval * 0.75f;
    if (vel.y >= 0.2f) explode();
    if (rv.length() > rocketLength) rv = rv.normalize() * rocketLength;
    rend.renderLine(sbuf, pos.x, pos.y, rocketColor, {0, 0}, rv);
    return true;
}

bool Firework::updateExplode(SplinterBuffer& sbuf, Renderer2D& rend,
                             float interval) {
    pos += vel * interval;
    vel.y += interval * 0.0625f;
    explodeColor_.a = static_cast<uint8_t>(explodeAlpha_ * 255);
    for (FireworkShard& piece : explodePieces_) {
        piece.pos += piece.vpos * interval;
        piece.rot += piece.vrot * interval;
        coord_t tx = shardLength * sin(piece.rot.x) * sin(piece.rot.y);
        coord_t ty = shardLength * cos(piece.rot.x) * sin(piece.rot.y);
        rend.renderLine(sbuf, pos.x, pos.y, explodeColor_, piece.pos,
                        piece.pos + Point2D(tx, ty));
    }
    explodeAlpha_ -= interval * explodeFade_ * 0.25f;
    return explodeAlpha_ > 0;
}

void NameEntry::gotMessage(const MenuMessage& msg) {
    switch (msg.type) {
        case MenuMessageType::MenuUp:
            sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            name_[index_] =
                remainder<int>(name_[index_] - 1, validScoreNameChars.size());
            redrawChar(index_);
            break;
        case MenuMessageType::MenuDown:
            sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            name_[index_] =
                remainder<int>(name_[index_] + 1, validScoreNameChars.size());
            redrawChar(index_);
            break;
        case MenuMessageType::MenuLeft:
            if (index_ > 0) {
                sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
                --index_;
                redrawChar(index_);
                redrawChar(index_ + 1);
            }
            break;
        case MenuMessageType::MenuSelect:
            if (index_ + 1 == highScoreNameLength) {
                [[fallthrough]];
                case MenuMessageType::MenuExit:
                    sendMessage(
                        AudioMessage::playSound(SoundEffect::MenuSelect));
                    running_ = false;
                    break;
            }
            [[fallthrough]];
        case MenuMessageType::MenuRight:
            if (index_ + 1 < highScoreNameLength) {
                sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
                ++index_;
                if (!nameok_[index_] &&
                    msg.type == MenuMessageType::MenuSelect) {
                    name_[index_] = name_[index_ - 1];
                }
                redrawChar(index_ - 1);
                redrawChar(index_);
                nameok_[index_] = true;
            }
            break;
        default:
            break;
    }
}

void NameEntry::redrawChar(size_t i) {
    SplinterBuffer& sbuf = namebuf_[i];
    std::string s(1, validScoreNameChars[name_[i]]);
    sbuf.clear();
    int ii = static_cast<int>(i);
    rendtext_.drawTextLineCenter(
        sbuf, 0.5 * (ii - 1), 0.25,
        ii == index_ ? Color{255, 255, 0, 255} : Color{255, 255, 255, 160}, s,
        6.0f);
}

static const std::array<float, 10> rankMul = {
    0.75f, 0.5f, 0.4f, 0.35f, 0.325f, 0.3f, 0.275f, 0.25f, 0.225f, 0.2f};

bool NameEntry::run(GameState& state, float interval) {
    if (!running_) {
        std::array<char, highScoreNameLength> n;
        for (int i = 0; i < highScoreNameLength; ++i)
            n[i] = validScoreNameChars[name_[i]];
        HighScoreEntry entry{n, entry_.score, entry_.cycles, entry_.endStage};
        int i = table_->insertHighscore(std::move(entry));
        dynamic_assert(i >= 0 && i < HighScoreTable::size, "wut?");
        sendMessage(AudioMessage::stopMusic());
        sendMessage(AudioMessage::playSound(SoundEffect::HighScoreEntered));
        sendMessage(HostMessage::enteredHighScore());
        sendMessage(LogicMessage::openHighScores(holder_, i, state.arcade));
        saveHighscores(*table_);
        return false;
    }

    if (state.arcade) {
        countdown_ -= interval;
        if (countdown_ < 0) {
            running_ = false;
        } else {
            rendtext_.drawTextLineRight(
                state.sbuf, 0.75, -0.125, Color{255, 255, 255, 255},
                std::to_string(static_cast<int>(countdown_)), 1.5);
        }
    }

    fireworks_.erase(std::remove_if(fireworks_.begin(), fireworks_.end(),
                                    [&](const std::unique_ptr<Firework>& fw) {
                                        return !fw->update(state.sbuf, rend2d_,
                                                           interval);
                                    }),
                     fireworks_.end());
    if (random(rd_firework_spawn) < 0.05f * rankMul[rank_])
        fireworks_.emplace_back(std::make_unique<Firework>());

    t_ = wrapAngle(t_ + interval * 4);
    uint8_t a = static_cast<uint8_t>(sin(t_) * 64 + 127);
    rendtext_.drawTextLineCenter(state.sbuf, 0, -0.625, Color{255, 255, 255, a},
                                 "NAME ENTRY", 2.5);
    state.sbuf.append(maintext_);
    for (size_t i = 0; i < namebuf_.size(); ++i) state.sbuf.append(namebuf_[i]);
    return true;
}

NameEntry::NameEntry(NameEntry&& move) noexcept
    : LogicModule(std::move(move)),
      holder_(std::move(move.holder_)),
      entry_(std::move(move.entry_)),
      table_(move.table_),
      name_(std::move(move.name_)),
      index_(move.index_),
      rendtext_(std::move(move.rendtext_)),
      rend2d_(std::move(move.rend2d_)),
      maintext_(std::move(move.maintext_)),
      namebuf_(std::move(move.namebuf_)),
      nameok_(std::move(move.nameok_)),
      fireworks_(std::move(move.fireworks_)) {}

NameEntry& NameEntry::operator=(NameEntry&& move) noexcept {
    LogicModule::operator=(std::move(move));
    holder_ = std::move(move.holder_);
    entry_ = std::move(move.entry_);
    table_ = move.table_;
    name_ = std::move(move.name_);
    index_ = move.index_;
    rendtext_ = std::move(move.rendtext_);
    rend2d_ = std::move(move.rend2d_);
    maintext_ = std::move(move.maintext_);
    namebuf_ = std::move(move.namebuf_);
    nameok_ = std::move(move.nameok_);
    fireworks_ = std::move(move.fireworks_);
    return *this;
}

NameEntry::NameEntry(std::shared_ptr<ModuleHolder> holder_,
                     const PartialHighScoreEntry& entry, HighScoreTable* table)
    : holder_(holder_), entry_(entry), table_(table) {
    rendtext_.setFont(getAssets().menuFont);
    for (size_t i = 0; i < name_.size(); ++i) {
        name_[i] = 0;
        redrawChar(i);
    }
    rank_ = table->getHighscoreRank(entry.score);
    rendtext_.drawTextLineCenter(
        maintext_, 0, -0.25,
        rank_ == 0 ? Color{255, 255, 0, 192} : Color{255, 255, 255, 192},
        rank_ == 0 ? "NEW HIGH SCORE!!"
                   : "YOUR RANK IS #" + std::to_string(rank_ + 1));
    rendtext_.drawTextLineCenter(maintext_, 0, -0.125,
                                 Color{255, 255, 255, 160},
                                 "ENTER YOUR INITIALS", 0.75);
    rendtext_.drawTextLineCenter(maintext_, 0, 0.625, Color{64, 255, 64, 192},
                                 std::to_string(entry.score), 0.75);
    sendMessage(AudioMessage::playMusic(MusicTrack::HighScore));
}

NameEntry::~NameEntry() noexcept {}

}  // namespace hiemalia
