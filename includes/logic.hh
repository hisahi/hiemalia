/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logic.hh: header file for logic module

#ifndef M_LOGIC_HH
#define M_LOGIC_HH

#include "defs.hh"
#include "hbase.hh"
#include "lvector.hh"
#include "module.hh"
#include "msg.hh"
#include "state.hh"

namespace hiemalia {

class LogicModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }
    virtual bool run(GameState& state, float interval) = 0;

    LogicModule(const HostModule& copy) = delete;
    LogicModule& operator=(const HostModule& copy) = delete;
    LogicModule(LogicModule&& move) : Module(std::move(move)) {}
    LogicModule& operator=(LogicModule&& move) {
        Module::operator=(std::move(move));
        return *this;
    }
    virtual ~LogicModule() {}

   protected:
    LogicModule() {}

   private:
    static inline const std::string role_ = "logic module";
};

enum class LogicMessageType { StartLogic };

enum class LogicModuleType { Test, Loading, Menu, Game };

struct LogicMessage {
    LogicMessageType type;
    union {
        LogicModuleType mtype;
        int later_;
    };

    static LogicMessage startLogic(LogicModuleType mtype) {
        return LogicMessage(LogicMessageType::StartLogic, mtype);
    }

   private:
    LogicMessage(LogicMessageType t, LogicModuleType mtype)
        : type(t), mtype(mtype) {}
};

class LogicEngine : public Module, MessageHandler<LogicMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    LogicEngine() {}

    void gotMessage(const LogicMessage& msg);
    void run(GameState& state, float interval);

   private:
    limited_vector<std::unique_ptr<LogicModule>, 16> modules_;
    static inline const std::string name_ = "LogicEngine";
    static inline const std::string role_ = "logic engine";
};

};  // namespace hiemalia

#endif  // M_LOGIC_HH
