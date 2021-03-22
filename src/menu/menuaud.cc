/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuinp.cc: implementation of input device menu

#include "menuaud.hh"

#include "audio.hh"
#include "controls.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "input.hh"
#include "logger.hh"
#include "menuinpb.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Music, Item_Sound, Item_Back };

void MenuAudioOptions::begin(GameState& state) {
    auto& config = holder_->audio->getConfig();
    state.config.sectionLoad(config);
    option(MenuOption::toggle(Item_Music, "MUSIC", config->music));
    option(MenuOption::toggle(Item_Sound, "SOUND", config->sound));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuAudioOptions::select(int index, symbol_t id) {
    AudioConfig& config = *holder_->audio->getConfig();
    switch (id) {
        case Item_Music:
            config.music = static_cast<bool>(options_[index].asSelect().index);
            if (!config.music) sendMessage(AudioMessage::stopMusic());
            break;
        case Item_Sound:
            config.sound = static_cast<bool>(options_[index].asSelect().index);
            if (!config.sound) sendMessage(AudioMessage::stopSounds());
            break;
        case Item_Back:
            closeMenu();
            break;
    }
}

void MenuAudioOptions::end(GameState& state) {
    state.config.sectionSave(holder_->audio->getConfig());
}

MenuAudioOptions::MenuAudioOptions(MenuAudioOptions&& move) noexcept
    : Menu(std::move(move)), holder_(std::move(move.holder_)) {}

MenuAudioOptions& MenuAudioOptions::operator=(
    MenuAudioOptions&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(holder_);
    return *this;
}

MenuAudioOptions::MenuAudioOptions(MenuHandler& handler,
                                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuAudioOptions::~MenuAudioOptions() {}

}  // namespace hiemalia
