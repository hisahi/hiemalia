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
    NewHighScore,
    OpenHighScores,
    StartGame,
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
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::MainMenu, holder);
    }

    inline static LogicMessage highScore(
        const std::shared_ptr<ModuleHolder>& holder, const HighScoreSubmit& e) {
        return LogicMessage(LogicMessageType::NewHighScore, holder, e);
    }

    inline static LogicMessage openHighScores(
        const std::shared_ptr<ModuleHolder>& holder, int i) {
        return LogicMessage(LogicMessageType::OpenHighScores, holder, i);
    }

    inline static LogicMessage startGame(
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::StartGame, holder);
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

  private:
    LogicMessage(LogicMessageType t) : type(t) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v)
        : type(t), value(v) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v,
                 const HighScoreSubmit& e)
        : type(t), value(v), x(e) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v, int i)
        : type(t), value(v), x(i) {}
    std::variant<HighScoreSubmit, int> x;
};

};  // namespace hiemalia

#endif  // M_LOGICMSG_HH
