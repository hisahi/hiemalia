/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuaud.hh: header file for menuaud.cc (audio option menu)

#ifndef M_MENUAUD_HH
#define M_MENUAUD_HH

#include <string>

#include "audio.hh"
#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuAudioOptions : public Menu {
   public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Audio"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuAudioOptions);
    MenuAudioOptions(MenuAudioOptions&& move) noexcept;
    MenuAudioOptions& operator=(MenuAudioOptions&& move) noexcept;
    MenuAudioOptions(MenuHandler& handler,
                     const std::shared_ptr<ModuleHolder>& holder);
    ~MenuAudioOptions() noexcept;

   private:
    static inline const std::string name_ = "MenuAudioOptions";
    std::shared_ptr<ModuleHolder> holder_;
};
};  // namespace hiemalia

#endif  // M_MENUAUD_HH
