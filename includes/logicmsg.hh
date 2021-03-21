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

namespace hiemalia {

enum class LogicMessageType { MainMenu, StartGame };

class ModuleHolder;

struct LogicMessage {
    LogicMessageType type;
    std::variant<std::shared_ptr<ModuleHolder>> value;

    inline static LogicMessage mainMenu(
        const std::shared_ptr<ModuleHolder>& holder) {
        return LogicMessage(LogicMessageType::MainMenu, holder);
    }

    inline const std::shared_ptr<ModuleHolder>& holder() const {
        return std::get<std::shared_ptr<ModuleHolder>>(value);
    }

    inline static LogicMessage startGame() {
        return LogicMessage(LogicMessageType::StartGame);
    }

   private:
    LogicMessage(LogicMessageType t) : type(t) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<ModuleHolder> v)
        : type(t), value(v) {}
};

};  // namespace hiemalia

#endif  // M_LOGICMSG_HH
