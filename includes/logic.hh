/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logic.hh: header file for logic engine

#ifndef M_LOGIC_HH
#define M_LOGIC_HH

#include <variant>

#include "defs.hh"
#include "hbase.hh"
#include "lmodule.hh"
#include "lvector.hh"
#include "menu.hh"
#include "module.hh"
#include "msg.hh"
#include "state.hh"

namespace hiemalia {

enum class LogicMessageType { StartMenu, StartGame };

struct LogicMessage {
    LogicMessageType type;
    std::variant<std::shared_ptr<Menu>> value;

    static LogicMessage startMenu(std::shared_ptr<Menu>&& menu) {
        return LogicMessage(LogicMessageType::StartMenu, std::move(menu));
    }

    template <typename T, typename... Ts>
    static LogicMessage startMenuNew(Ts&&... args) {
        return LogicMessage(LogicMessageType::StartMenu,
                            std::make_shared<T>(std::forward<Ts>(args)...));
    }

    const std::shared_ptr<Menu>& menu() const {
        dynamic_assert(type == LogicMessageType::StartMenu, "invalid access");
        return std::get<std::shared_ptr<Menu>>(value);
    }

    static LogicMessage startGame() {
        return LogicMessage(LogicMessageType::StartGame);
    }

   private:
    LogicMessage(LogicMessageType t) : type(t) {}
    LogicMessage(LogicMessageType t, std::shared_ptr<Menu>&& menu)
        : type(t), value(std::move(menu)) {}
};

class LogicEngine : public Module, MessageHandler<LogicMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    LogicEngine() {}
    DELETE_COPY(LogicEngine);
    INHERIT_MOVE(LogicEngine, Module);

    void gotMessage(const LogicMessage& msg);
    void run(GameState& state, float interval);
    void test();

   private:
    LimitedVector<std::shared_ptr<LogicModule>, 16> modules_;
    static inline const std::string name_ = "LogicEngine";
    static inline const std::string role_ = "logic engine";
};

};  // namespace hiemalia

#endif  // M_LOGIC_HH
