/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu.hh: header file for the menus (menu.cc)

#ifndef M_MENU_HH
#define M_MENU_HH

#include <functional>
#include <optional>
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
    MenuExit,
    MenuYes,
    MenuNo,
};

struct MenuMessage {
    MenuMessageType type;
    symbol_t menuId;

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
    static MenuMessage yes(symbol_t id) {
        return MenuMessage(MenuMessageType::MenuYes, id);
    }
    static MenuMessage no(symbol_t id) {
        return MenuMessage(MenuMessageType::MenuNo, id);
    }

  private:
    MenuMessage(MenuMessageType t) : type(t), menuId(symbol_none) {}
    MenuMessage(MenuMessageType t, symbol_t id) : type(t), menuId(id) {}
};

enum class MenuOptionType { Spacer, Button, Text, Select, Spinner, Input };

struct MenuOptionSelect {
    int index{0};
    std::vector<std::string> options;
};

struct MenuOptionSpinner {
    int value{0};
    int min{0};
    int max{0};
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

    inline static MenuOption spacer(symbol_t id = symbol_none) {
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

    inline static MenuOption select(symbol_t id, std::string text,
                                    std::initializer_list<std::string> options,
                                    int initialValue = 0, bool enabled = true) {
        if (initialValue >= static_cast<int>(options.size())) initialValue = 0;
        return MenuOption(id, MenuOptionType::Select, enabled, text,
                          MenuOptionSelect{initialValue, options});
    }

    inline static MenuOption range(symbol_t id, std::string text, int min,
                                   int max, int initialValue = 0,
                                   bool enabled = true) {
        if (min > max) std::swap(min, max);
        initialValue = clamp(min, initialValue, max);
        return MenuOption(id, MenuOptionType::Spinner, enabled && max > min,
                          text, MenuOptionSpinner{initialValue, min, max});
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
    inline MenuOptionSpinner& asSpinner() {
        return std::get<MenuOptionSpinner>(x);
    }

    inline static bool defaultEnabled(MenuOptionType type) {
        return type != MenuOptionType::Spacer && type != MenuOptionType::Text;
    }

    void redraw(coord_t x1, coord_t x2, coord_t y, RendererText& font,
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
    MenuOption(symbol_t id, MenuOptionType type, bool enabled, std::string text,
               MenuOptionSpinner spin)
        : id(id),
          type(type),
          enabled(enabled),
          text(text),
          dirty(true),
          x(spin) {
        dynamic_assert(type == MenuOptionType::Spinner, "invalid init");
    }
    std::variant<MenuOptionSelect, MenuOptionInput, MenuOptionSpinner> x;
};

enum class MainMenuEnterFlag { MainMenu, HighScore, FromDemo };

struct MenuInformation {
    bool arcade{false};
    MainMenuEnterFlag mainMenuMode{MainMenuEnterFlag::MainMenu};
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
    MenuHandler(const MenuInformation& = {});
    ~MenuHandler() noexcept {}

    template <typename T = Menu>
    T& openMenu(const std::shared_ptr<T>& menu) {
        menus_.push(menu);
        return *menu;
    }

    template <typename T, typename... Ts>
    void newMenu(Ts&&... args) {
        menus_.push(std::make_shared<T>(*this, std::forward<Ts>(args)...));
    }

    inline std::shared_ptr<Menu> replaceMenu(std::shared_ptr<Menu>&& m) {
        auto s = menus_.top();
        menus_.pop();
        menus_.push(std::move(m));
        return s;
    }

    std::shared_ptr<Menu> closeMenu();

    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);
    inline const MenuInformation& info() const noexcept { return info_; }
    void closeAllMenus();

  private:
    std::stack<std::shared_ptr<Menu>> menus_;
    MenuInformation info_;

    static inline const std::string name_ = "MenuHandler";
    static inline const std::string role_ = "menu handler";
};

class Menu {
  public:
    virtual std::string title() const noexcept = 0;
    virtual void begin(GameState& state) = 0;
    virtual void select(int index, symbol_t id) = 0;
    virtual void end(GameState& state) = 0;

    inline void option(MenuOption&& option) {
        options_.push_back(std::move(option));
    }
    inline void setActiveItem(symbol_t item);
    inline void gotMenuMessage(const MenuMessage& msg);
    inline void runMenu(GameState& state, float interval);
    virtual inline void renderSpecial(SplinterBuffer& sbuf, float interval) {}

    DELETE_COPY(Menu);
    Menu(Menu&& move) noexcept;
    Menu& operator=(Menu&& move) noexcept;
    virtual ~Menu() noexcept {}

    inline void closeMenu() { exiting_ = true; }
    void confirm(symbol_t id, const std::string& title,
                 const std::function<void()>& onYes,
                 const std::function<void()>& onNo);

    template <typename T, typename... Ts>
    void openMenu(Ts&&... args) {
        handler_.get().newMenu<T>(std::forward<Ts>(args)...);
    }

    template <typename T, typename... Ts>
    void replaceMenu(Ts&&... args) {
        replaceWith_ =
            std::make_shared<T>(handler_.get(), std::forward<Ts>(args)...);
        closeMenu();
    }

    inline void checkTimeout() {
        if (timeout()) timedOut();
    }

  protected:
    Menu(MenuHandler& handler);
    int index_{0};
    symbol_t defaultItem{symbol_none};
    bool init_{false};
    bool exiting_{false};
    bool exitable_{true};
    inline void makeUncloseable() { exitable_ = false; }
    std::reference_wrapper<MenuHandler> handler_;
    RendererText font_;
    SplinterBuffer titlebuf_;
    std::vector<MenuOption> options_;
    std::shared_ptr<Menu> replaceWith_;
    coord_t getMenuOptionY(size_t index) const;
    inline virtual void pageLeft() {}
    inline virtual void pageRight(bool) {}
    inline const MenuInformation& info() const noexcept {
        return handler_.get().info();
    }
    inline bool timeout() const {
        return maxLifetime_ && lifetime_ >= maxLifetime_;
    }
    inline void duration(float t) {
        lifetime_ = 0, maxLifetime_ = info().arcade ? t : 0;
    }
    inline virtual void timedOut() { exiting_ = true; }

  private:
    void goUp();
    void goDown();
    void doLeft();
    void doRight();
    void doSelect();
    void doConfirm(bool yes);
    float lifetime_{0};
    float maxLifetime_{0};
    symbol_t confirmId_{symbol_none};
    std::optional<std::function<void()>> onMenuYes_;
    std::optional<std::function<void()>> onMenuNo_;
};
};  // namespace hiemalia

#endif  // M_MENU_HH
