/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu.hh: header file for the menus (menu.cc)

#ifndef M_MENU_HH
#define M_MENU_HH

#include <string>

#include "defs.hh"
#include "inherit.hh"
#include "lmodule.hh"
#include "msg.hh"
#include "rendtext.hh"
#include "state.hh"
#include "symbol.hh"

namespace hiemalia {
enum class MenuMessageType {
    MenuUp,
    MenuDown,
    MenuLeft,
    MenuRight,
    MenuSelect,
    MenuExit
};

struct MenuMessage {
    MenuMessageType type;

    static MenuMessage up() { return MenuMessage(MenuMessageType::MenuUp); }
    static MenuMessage down() { return MenuMessage(MenuMessageType::MenuDown); }
    static MenuMessage left() { return MenuMessage(MenuMessageType::MenuLeft); }
    static MenuMessage right() {
        return MenuMessage(MenuMessageType::MenuRight);
    }
    static MenuMessage select() {
        return MenuMessage(MenuMessageType::MenuSelect);
    }
    static MenuMessage exit() { return MenuMessage(MenuMessageType::MenuExit); }

   private:
    MenuMessage(MenuMessageType t) : type(t) {}
};

enum class MenuOptionType { Button, Text, Select };

struct MenuOptionSelect {
    int index;
    std::vector<std::string> options;
};

class MenuOption {
   public:
    symbol_t id;
    MenuOptionType type;
    bool enabled;
    std::string text;
    std::variant<MenuOptionSelect> x;
    bool dirty;
    SplinterBuffer sbuf;

    static MenuOption make_button(symbol_t id, std::string text,
                                  bool enabled = true) {
        return MenuOption(id, MenuOptionType::Button, enabled, text);
    }

    static MenuOption make_spacer(symbol_t id) {
        return MenuOption(id, MenuOptionType::Text, false, "");
    }

    static MenuOption make_text(symbol_t id, std::string text) {
        return MenuOption(id, MenuOptionType::Text, false, text);
    }

    void redraw(coord_t x1, coord_t x2, coord_t y, const RendererText& font,
                const Color& color);

   private:
    MenuOption(symbol_t id, MenuOptionType type, bool enabled, std::string text)
        : id(id), type(type), enabled(enabled), text(text), dirty(true) {}
};

class Menu : public LogicModule, MessageHandler<MenuMessage> {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }

    virtual void begin(GameState& state) = 0;
    virtual void select(int index, symbol_t id) = 0;
    virtual void end(GameState& state) = 0;

    void option(MenuOption&& option) { options_.push_back(std::move(option)); }
    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);

    DELETE_COPY(Menu);
    Menu(Menu&& move) noexcept;
    Menu& operator=(Menu&& move) noexcept;
    virtual ~Menu() noexcept {}

   protected:
    Menu(std::shared_ptr<Menu>&& parent);
    int index_{0};
    bool init_{false};
    bool exiting_{false};
    std::shared_ptr<Menu> parent_;
    RendererText font_;
    std::vector<MenuOption> options_;

   private:
    static inline const std::string role_ = "menu module";
    void goUp();
    void goDown();
    void doLeft();
    void doRight();
};
};  // namespace hiemalia

#endif  // M_MENU_HH
