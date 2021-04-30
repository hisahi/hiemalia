/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuvid.cc: implementation of video device menu

#include "menu/menuvid.hh"

#include <utility>

#include "audio.hh"
#include "controls.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "input.hh"
#include "logger.hh"

namespace hiemalia {

enum Item : symbol_t { Item_FullScreen, Item_Back };

void MenuVideoOptions::begin(GameState& state) {
    auto& video = holder_->video;
    const auto& config = holder_->video->getConfig();
    state.config.sectionLoad(config);
    option(MenuOption::toggle(Item_FullScreen, "FULL SCREEN",
                              video->isFullScreen(),
                              video->canSetFullScreen()));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuVideoOptions::select(int index, symbol_t id) {
    VideoConfig& config = *holder_->video->getConfig();
    switch (id) {
        case Item_FullScreen:
            config.fullScreen =
                static_cast<bool>(options_[index].asSelect().index);
            break;
        case Item_Back:
            closeMenu();
            break;
    }
}

void MenuVideoOptions::end(GameState& state) {
    state.config.sectionSave(holder_->video->getConfig());
    holder_->video->readConfig();
}

MenuVideoOptions::MenuVideoOptions(MenuHandler& handler,
                                   std::shared_ptr<ModuleHolder> holder)
    : Menu(handler), holder_(std::move(holder)) {}

}  // namespace hiemalia
