/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// ibase.hh: header file for base input library

#ifndef M_IBASE_HH
#define M_IBASE_HH

#include <string>

#include "buttons.hh"
#include "config.hh"
#include "hbase.hh"
#include "module.hh"

namespace hiemalia {
class InputControlModule;

class InputModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }

    virtual void update(ControlState& state, MenuControlState& menustate) = 0;
    virtual bool hasInputDevice(InputDevice device) const = 0;
    // these MUST be destroyed before/with the InputModule!
    virtual InputControlModule& addInputDevice(
        InputDevice device, ConfigSectionPtr<ButtonSetup>& config) = 0;

    InputModule(const InputModule& copy) = delete;
    InputModule& operator=(const InputModule& copy) = delete;
    InputModule(InputModule&& move) : Module(std::move(move)) {}
    InputModule& operator=(InputModule&& move) {
        Module::operator=(std::move(move));
        return *this;
    }
    virtual ~InputModule() {}

   protected:
    InputModule() {}

   private:
    static inline const std::string role_ = "input module";
};

class InputControlModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }
    InputDevice device() const noexcept { return device_; }

    virtual std::string getButtonName(ControlInput input) const = 0;
    virtual void setNextButton(ControlInput input) = 0;
    virtual void resetDefault(ControlInput input) = 0;
    virtual void resetAllDefaults() = 0;
    virtual bool isSettingKey() = 0;

    InputControlModule(const InputControlModule& copy) = delete;
    InputControlModule& operator=(const InputControlModule& copy) = delete;
    InputControlModule(InputControlModule&& move)
        : Module(std::move(move)),
          device_(move.device_),
          config_(std::move(move.config_)) {}
    InputControlModule& operator=(InputControlModule&& move) {
        Module::operator=(std::move(move));
        device_ = move.device_;
        config_ = std::move(move.config_);
        return *this;
    }
    virtual ~InputControlModule() {}

   protected:
    explicit InputControlModule(InputModule& host,
                                const ConfigSectionPtr<ButtonSetup>& config)
        : device_(config->getDevice()), config_(config) {}
    InputDevice device_;
    ConfigSectionPtr<ButtonSetup> config_;

   private:
    static inline const std::string role_ = "input control module";
};

std::shared_ptr<InputModule> getInputModule(std::shared_ptr<HostModule> host);

};  // namespace hiemalia

#endif  // M_IBASE_HH
