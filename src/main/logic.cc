/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logic.cc: implementation of LogicEngine

#include "logic.hh"

#include "defs.hh"
#include "game/game.hh"
#include "menumain.hh"
#include "menupaus.hh"

namespace hiemalia {
void LogicEngine::gotMessage(const LogicMessage& msg) {
    switch (msg.type) {
        case LogicMessageType::MainMenu: {
            MenuHandler& menu = getOrCreate<MenuHandler>();
            menu.openMenu(std::make_shared<MenuMain>(menu, msg.holder()));
            break;
        }
        case LogicMessageType::StartGame: {
            getOrCreate<GameMain>();
            break;
        }
        case LogicMessageType::PauseMenu: {
            MenuHandler& menu = getOrCreate<MenuHandler>();
            menu.openMenu(std::make_shared<MenuPause>(menu));
            break;
        }
    }
}

void LogicEngine::run(GameState& state, float interval) {
    auto it = modules_.begin();
    while (it != modules_.end()) {
        if ((*it)->run(state, interval))
            ++it;
        else {
            it = modules_.erase(it);
        }
    }
}

}  // namespace hiemalia
