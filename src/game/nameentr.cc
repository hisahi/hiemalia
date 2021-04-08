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

namespace hiemalia {
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
                    continue_ = false;
                    break;
            }
            [[fallthrough]];
        case MenuMessageType::MenuRight:
            if (index_ + 1 < highScoreNameLength) {
                sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
                ++index_;
                redrawChar(index_ - 1);
                redrawChar(index_);
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
        4.0f);
}

bool NameEntry::run(GameState& state, float interval) {
    if (!continue_) {
        std::array<char, highScoreNameLength> n;
        for (int i = 0; i < highScoreNameLength; ++i)
            n[i] = validScoreNameChars[name_[i]];
        HighScoreEntry entry{n, entry_.score, entry_.cycles, entry_.endStage};
        int i = table_->insertHighscore(std::move(entry));
        dynamic_assert(i >= 0 && i < HighScoreTable::size, "wut?");
        sendMessage(AudioMessage::fadeOutMusic());
        sendMessage(LogicMessage::openHighScores(holder_, i));
        saveHighscores(*table_);
        return false;
    }
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
      rendtext_(std::move(move.rendtext_)) {}

NameEntry& NameEntry::operator=(NameEntry&& move) noexcept {
    LogicModule::operator=(std::move(move));
    holder_ = std::move(move.holder_);
    entry_ = std::move(move.entry_);
    table_ = move.table_;
    name_ = std::move(move.name_);
    index_ = move.index_;
    rendtext_ = std::move(move.rendtext_);
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
    int rank = table->getHighscoreRank(entry.score);
    rendtext_.drawTextLineCenter(
        maintext_, 0, -0.25,
        rank == 0 ? Color{255, 255, 0, 192} : Color{255, 255, 255, 192},
        rank == 0 ? "NEW HIGH SCORE!!"
                  : "YOUR RANK IS #" + std::to_string(rank + 1));
    rendtext_.drawTextLineCenter(maintext_, 0, -0.125,
                                 Color{255, 255, 255, 160},
                                 "ENTER YOUR INITIALS", 0.75);
    rendtext_.drawTextLineCenter(maintext_, 0, 0.625, Color{64, 255, 64, 192},
                                 std::to_string(entry.score), 0.75);
    sendMessage(AudioMessage::playMusic(MusicTrack::HighScore));
}

NameEntry::~NameEntry() noexcept {}

}  // namespace hiemalia
