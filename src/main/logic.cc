/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logic.cc: implementation of LogicEngine

#include "logic.hh"

#include "defs.hh"
#include "game/demo.hh"
#include "game/game.hh"
#include "game/nameentr.hh"
#include "hiemalia.hh"
#include "menu/menuhigh.hh"
#include "menu/menumain.hh"
#include "menu/menupaus.hh"

namespace hiemalia {
void LogicEngine::gotMessage(const LogicMessage& msg) {
    switch (msg.type) {
        case LogicMessageType::MainMenu: {
            MenuHandler& menu = getOrCreate<MenuHandler>(MenuInformation{
                msg.isArcade(), !msg.isArcade() || firstRun_
                                    ? MainMenuEnterFlag::MainMenu
                                    : MainMenuEnterFlag::HighScore});
            menu.openMenu(std::make_shared<MenuMain>(menu, msg.holder()));
            if (!firstRun_)
                menu.openMenu<MenuHighScore>(
                    std::make_shared<MenuHighScore>(menu, msg.holder()));
            firstRun_ = false;
            break;
        }
        case LogicMessageType::MainMenuFromDemo: {
            MenuHandler& menu = getOrCreate<MenuHandler>(
                MenuInformation{msg.isArcade(), MainMenuEnterFlag::FromDemo});
            menu.openMenu(std::make_shared<MenuMain>(menu, msg.holder()));
            menu.openMenu<MenuHighScore>(
                std::make_shared<MenuHighScore>(menu, msg.holder()));
            break;
        }
        case LogicMessageType::OpenHighScores: {
            MenuHandler& menu = getOrCreate<MenuHandler>(
                MenuInformation{msg.isArcade(), MainMenuEnterFlag::HighScore});
            menu.openMenu(std::make_shared<MenuMain>(menu, msg.holder()));
            menu.openMenu<MenuHighScore>(
                    std::make_shared<MenuHighScore>(menu, msg.holder()))
                .highlight(msg.highScoreIndex());
            break;
        }
        case LogicMessageType::NewHighScore: {
            getOrCreate<NameEntry>(msg.holder(), msg.highScoreEntry().entry,
                                   msg.highScoreEntry().table);
            break;
        }
        case LogicMessageType::StartGameArcade:
            if (auto* m = getOrNull<MenuHandler>()) m->closeAllMenus();
            [[fallthrough]];
        case LogicMessageType::StartGame:
            getOrCreate<GameMain>(msg.holder()->gconfig, nullptr);
            sendMessage(HostMessage::gameStarted());
            break;
        case LogicMessageType::StartDemo:
            sendMessage(AudioMessage::mute());
            getOrCreate<GameMain>(msg.holder()->gconfig, getNextDemo());
            sendMessage(HostMessage::demoStarted());
            break;
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
