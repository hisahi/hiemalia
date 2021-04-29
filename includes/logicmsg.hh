/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logicmsg.hh: header file for logic message

#ifndef M_LOGICMSG_HH
#define M_LOGICMSG_HH

#include <algorithm>
#include <variant>

#include "defs.hh"
#include "hbase.hh"
#include "menu.hh"
#include "module.hh"
#include "msg.hh"
#include "scores.hh"

namespace hiemalia {

enum class LogicMessageType {
    MainMenu,
    MainMenuFromDemo,
    NewHighScore,
    OpenHighScores,
    StartGame,
    StartGameArcade,
    StartDemo,
    PauseMenu
};

struct ModuleHolder;

struct HighScoreSubmit {
    HighScoreTable* table;
    PartialHighScoreEntry entry;
};

struct LogicMessage {
    LogicMessageType type;
    std::variant<std::shared_ptr<ModuleHolder>> value;

    inline const std::shared_ptr<ModuleHolder>& holder() const {
        return std::get<std::shared_ptr<ModuleHolder>>(value);
    }

    inline static LogicMessage mainMenu(
        const std::shared_ptr<ModuleHolder>& holder, bool arcade) {
        return LogicMessage(LogicMessageType::MainMenu, holder, arcade);
    }

    inline static LogicMessage mainMenuFromDemo(
        const std::shared_ptr<ModuleHolder>& holder, bool arcade) {
        return LogicMessage(LogicMessageType::MainMenuFromDemo, holder, arcade);
    }

    inline static LogicMessage highScore(
        const std::shared_ptr<ModuleHolder>& holder, const HighScoreSubmit& e) {
        return LogicMessage(LogicMessageType::NewHighScore, holder, e);
    }

    inline static LogicMessage openHighScores(
        const std::shared_ptr<ModuleHolder>& holder, int i, bool arcade) {
        return LogicMessage(LogicMessageType::OpenHighScores, holder, i,
                            arcade);
    }

    inline static LogicMessage startGame(
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::StartGame, holder);
    }

    inline static LogicMessage startGameArcade(
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::StartGameArcade, holder);
    }

    inline static LogicMessage startDemo(
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::StartDemo, holder);
    }

    inline static LogicMessage pauseMenu() {
        return LogicMessage(LogicMessageType::PauseMenu);
    }

    inline const HighScoreSubmit& highScoreEntry() const {
        dynamic_assert(type == LogicMessageType::NewHighScore,
                       "wrong message type");
        return std::get<HighScoreSubmit>(x);
    }

    inline int highScoreIndex() const {
        dynamic_assert(type == LogicMessageType::OpenHighScores,
                       "wrong message type");
        return std::get<int>(x);
    }
    inline bool isArcade() const {
        dynamic_assert(type == LogicMessageType::MainMenu ||
                           type == LogicMessageType::MainMenuFromDemo ||
                           type == LogicMessageType::OpenHighScores,
                       "wrong message type");
        return arcade;
    }

  private:
    LogicMessage(LogicMessageType t) : type(t) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v)
        : type(t), value(v) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v,
                 const HighScoreSubmit& e)
        : type(t), value(v), x(e) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v, bool b)
        : type(t), value(v), arcade(b) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v, int i,
                 bool b)
        : type(t), value(v), x(i), arcade(b) {}
    std::variant<HighScoreSubmit, int> x;
    bool arcade{false};
};

};  // namespace hiemalia

#endif  // M_LOGICMSG_HH
