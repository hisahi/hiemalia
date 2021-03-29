/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/ibasei.hh: header file for SDL2 input module implementation

#ifndef M_BASE_SDL2_IBASEI_HH
#define M_BASE_SDL2_IBASEI_HH

#include <memory>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "buttons.hh"
#include "ibase.hh"
#include "inherit.hh"

namespace hiemalia {
class InputControlModuleSDL2;

class InputModuleSDL2 : public InputModule {
   public:
    std::string name() const noexcept { return name_; }

    void update(ControlState& state, MenuControlState& menustate);
    bool hasInputDevice(InputDevice device) const;
    InputControlModule& getInputDevice(InputDevice device);
    InputControlModule& addInputDevice(
        InputDevice device, const ConfigSectionPtr<ButtonSetup>& config);
    void handle(const SDL_Event& event);

    explicit InputModuleSDL2(std::shared_ptr<HostModule> host);
    DELETE_COPY(InputModuleSDL2);
    InputModuleSDL2(InputModuleSDL2&& move) noexcept;
    InputModuleSDL2& operator=(InputModuleSDL2&& move) noexcept;
    ~InputModuleSDL2() noexcept;

   private:
    static inline const std::string name_ = "InputModuleSDL2";
    std::shared_ptr<HostModuleSDL2> host_;
    std::vector<std::unique_ptr<InputControlModuleSDL2>> devices_;
    ControlState state_;
    MenuControlState menustate_;
    bool subsystem_{false};
};

class InputControlModuleSDL2 : public InputControlModule {
   public:
    std::string name() const noexcept { return name_; }

    void handle(ControlState& state, MenuControlState& menustate,
                const SDL_Event& event);
    std::string getButtonName(ControlInput input) const;
    void setNextButton(ControlInput input);
    void resetDefault(ControlInput input);
    void resetAllDefaults();
    bool isSettingKey() { return setting_key_; }

    explicit InputControlModuleSDL2(
        InputModuleSDL2& host, const ConfigSectionPtr<ButtonSetup>& config);
    DELETE_COPY(InputControlModuleSDL2);
    InputControlModuleSDL2(InputControlModuleSDL2&& move) noexcept;
    InputControlModuleSDL2& operator=(InputControlModuleSDL2&& move) noexcept;
    ~InputControlModuleSDL2() noexcept;

   private:
    static inline const std::string name_ = "InputControlModuleSDL2";
    SDL_GameController* pad_{nullptr};
    SDL_Joystick* joy_{nullptr};
    InputModuleSDL2& host_;
    bool setting_key_{false};
    ControlInput set_next_;
    friend InputControlModule& InputModuleSDL2::addInputDevice(
        InputDevice, const ConfigSectionPtr<ButtonSetup>&);
};
};  // namespace hiemalia

#endif  // M_BASE_SDL2_IBASEI_HH
