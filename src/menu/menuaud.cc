/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuaud.cc: implementation of audio device menu

#include "menu/menuaud.hh"

#include "audio.hh"
#include "controls.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "input.hh"
#include "logger.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Music, Item_Sound, Item_Back };

void MenuAudioOptions::begin(GameState& state) {
    auto& audio = holder_->audio;
    auto& config = holder_->audio->getConfig();
    state.config.sectionLoad(config);
    option(MenuOption::toggle(Item_Music, "MUSIC",
                              audio->canPlayMusic() && config->music,
                              audio->canPlayMusic()));
    option(MenuOption::toggle(Item_Sound, "SOUND",
                              audio->canPlaySound() && config->sound,
                              audio->canPlaySound()));
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

MenuAudioOptions::MenuAudioOptions(MenuHandler& handler,
                                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuAudioOptions::~MenuAudioOptions() noexcept {}

}  // namespace hiemalia
