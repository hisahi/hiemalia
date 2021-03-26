/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu.hh: header file for the menus (menu.cc)

#ifndef M_MENU_HH
#define M_MENU_HH

#include <stack>
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

enum class MenuOptionType { Spacer, Button, Text, Select, Input };

struct MenuOptionSelect {
    int index;
    std::vector<std::string> options;
};

struct MenuOptionInput {
    std::string value;
};

class MenuOption {
   public:
    symbol_t id;
    MenuOptionType type;
    bool enabled;
    std::string text;
    bool dirty;
    SplinterBuffer sbuf;

    inline static MenuOption button(symbol_t id, std::string text,
                                    bool enabled = true) {
        return MenuOption(id, MenuOptionType::Button, enabled, text);
    }

    inline static MenuOption spacer(symbol_t id) {
        return MenuOption(id, MenuOptionType::Spacer, false, "");
    }

    inline static MenuOption label(symbol_t id, std::string text) {
        return MenuOption(id, MenuOptionType::Text, false, text);
    }

    inline static MenuOption toggle(symbol_t id, std::string text,
                                    bool initialValue = false,
                                    bool enabled = true) {
        return MenuOption(id, MenuOptionType::Select, enabled, text,
                          MenuOptionSelect{initialValue != 0, {"OFF", "ON"}});
    }

    inline static MenuOption input(symbol_t id, std::string text,
                                   std::string initialValue = "",
                                   bool enabled = true) {
        return MenuOption(id, MenuOptionType::Input, enabled, text,
                          MenuOptionInput{initialValue});
    }

    inline MenuOptionInput& asInput() { return std::get<MenuOptionInput>(x); }
    inline MenuOptionSelect& asSelect() {
        return std::get<MenuOptionSelect>(x);
    }

    inline static bool defaultEnabled(MenuOptionType type) {
        return type != MenuOptionType::Spacer && type != MenuOptionType::Text;
    }

    void redraw(coord_t x1, coord_t x2, coord_t y, const RendererText& font,
                const Color& color);

   private:
    MenuOption(symbol_t id, MenuOptionType type, bool enabled, std::string text)
        : id(id), type(type), enabled(enabled), text(text), dirty(true) {}
    MenuOption(symbol_t id, MenuOptionType type, bool enabled, std::string text,
               MenuOptionSelect sel)
        : id(id),
          type(type),
          enabled(enabled),
          text(text),
          dirty(true),
          x(sel) {
        dynamic_assert(type == MenuOptionType::Select, "invalid init");
    }
    MenuOption(symbol_t id, MenuOptionType type, bool enabled, std::string text,
               MenuOptionInput inp)
        : id(id),
          type(type),
          enabled(enabled),
          text(text),
          dirty(true),
          x(inp) {
        dynamic_assert(type == MenuOptionType::Input, "invalid init");
    }
    std::variant<MenuOptionSelect, MenuOptionInput> x;
};

class Menu;

class MenuHandler : public LogicModule, MessageHandler<MenuMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    DELETE_COPY(MenuHandler);

    MenuHandler(MenuHandler&& move) noexcept
        : LogicModule(std::move(move)), menus_(std::move(move.menus_)) {}
    MenuHandler& operator=(MenuHandler&& move) noexcept {
        Module::operator=(std::move(move));
        menus_ = std::move(move.menus_);
        return *this;
    }
    MenuHandler();
    ~MenuHandler() noexcept {}

    void openMenu(const std::shared_ptr<Menu>& menu) {
        menus_.push(std::move(menu));
    }

    template <typename T, typename... Ts>
    void newMenu(Ts&&... args) {
        menus_.push(std::make_shared<T>(*this, std::forward<Ts>(args)...));
    }

    std::shared_ptr<Menu> closeMenu() {
        auto s = menus_.top();
        menus_.pop();
        return s;
    }

    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);

   private:
    std::stack<std::shared_ptr<Menu>> menus_;
    static inline const std::string name_ = "MenuHandler";
    static inline const std::string role_ = "menu handler";
};

class Menu {
   public:
    virtual std::string title() const noexcept = 0;
    virtual void begin(GameState& state) = 0;
    virtual void select(int index, symbol_t id) = 0;
    virtual void end(GameState& state) = 0;

    void option(MenuOption&& option) { options_.push_back(std::move(option)); }
    void gotMenuMessage(const MenuMessage& msg);
    void runMenu(GameState& state, float interval);

    DELETE_COPY(Menu);
    Menu(Menu&& move) noexcept;
    Menu& operator=(Menu&& move) noexcept;
    virtual ~Menu() noexcept {}

    void closeMenu() { exiting_ = true; }

    template <typename T, typename... Ts>
    void openMenu(Ts&&... args) {
        handler_.get().newMenu<T>(std::forward<Ts>(args)...);
    }

   protected:
    Menu(MenuHandler& handler);
    int index_{0};
    bool init_{false};
    bool exiting_{false};
    bool exitable_{true};
    inline void makeUncloseable() { exitable_ = false; }
    std::reference_wrapper<MenuHandler> handler_;
    RendererText font_;
    SplinterBuffer titlebuf_;
    std::vector<MenuOption> options_;

   private:
    void goUp();
    void goDown();
    void doLeft();
    void doRight();
    void doSelect();
};
};  // namespace hiemalia

#endif  // M_MENU_HH
