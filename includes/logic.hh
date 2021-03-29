/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logic.hh: header file for logic engine

#ifndef M_LOGIC_HH
#define M_LOGIC_HH

#include <algorithm>
#include <variant>

#include "defs.hh"
#include "hbase.hh"
#include "lmodule.hh"
#include "logicmsg.hh"
#include "lvector.hh"
#include "menu.hh"
#include "module.hh"
#include "msg.hh"
#include "state.hh"

namespace hiemalia {

class LogicEngine : public Module, MessageHandler<LogicMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    LogicEngine() {}
    DELETE_COPY(LogicEngine);
    INHERIT_MOVE(LogicEngine, Module);

    void gotMessage(const LogicMessage& msg);
    void run(GameState& state, float interval);

    template <typename T, typename... Ts>
    T& getOrCreate(Ts&&... args) {
        auto it =
            std::find_if(modules_.begin(), modules_.end(),
                         [&](const std::shared_ptr<LogicModule>& module) {
                             return dynamic_cast<T*>(module.get()) != nullptr;
                         });
        return dynamic_cast<T&>(
            it != modules_.end() ? **it
                                 : *modules_.emplace_back(std::make_shared<T>(
                                       std::forward<Ts>(args)...)));
    }

   private:
    LimitedVector<std::shared_ptr<LogicModule>, 16> modules_;
    static inline const std::string name_ = "LogicEngine";
    static inline const std::string role_ = "logic engine";
};

};  // namespace hiemalia

#endif  // M_LOGIC_HH
