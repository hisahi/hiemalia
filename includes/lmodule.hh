/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// lmodule.hh: header file for logic module

#ifndef M_LMODULE_HH
#define M_LMODULE_HH

#include <variant>

#include "defs.hh"
#include "hbase.hh"
#include "module.hh"
#include "state.hh"

namespace hiemalia {

class LogicModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    virtual std::string role() const noexcept { return role_; }
    virtual bool run(GameState& state, float interval) = 0;

    LogicModule(const LogicModule& copy) = delete;
    LogicModule& operator=(const LogicModule& copy) = delete;
    LogicModule(LogicModule&& move) noexcept : Module(std::move(move)) {}
    LogicModule& operator=(LogicModule&& move) noexcept {
        Module::operator=(std::move(move));
        return *this;
    }
    virtual ~LogicModule() noexcept {}

   protected:
    LogicModule() {}

   private:
    static inline const std::string role_ = "logic module";
};

};  // namespace hiemalia

#endif  // M_LMODULE_HH
