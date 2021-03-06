/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu.cc: implementation of Menu

#include "menu.hh"

#include "assets.hh"
#include "audio.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "inherit.hh"
#include "load2d.hh"
#include "logger.hh"
#include "logic.hh"
#include "menu/menumain.hh"
#include "menu/menuyn.hh"
#include "sounds.hh"

namespace hiemalia {

static const Color menuTitleColor{255, 255, 255, 255};
static const Color menuItemSelectedColor{255, 255, 255, 255};
static const Color menuItemColor{255, 255, 255, 160};
static const Color menuItemDisabledColor{255, 255, 255, 96};

void MenuOption::redraw(coord_t x1, coord_t x2, coord_t y, RendererText& font,
                        const Color& color) {
    sbuf.clear();
    switch (type) {
        case MenuOptionType::Spacer:
            break;
        case MenuOptionType::Text:
            font.drawTextLineLeft(sbuf, x1, y, color, text);
            break;
        case MenuOptionType::Button:
            font.drawTextLineCenter(sbuf, (x1 + x2) / 2, y, color, text);
            break;
        case MenuOptionType::Select: {
            std::string v = asSelect().options.at(asSelect().index);
            font.drawTextLineLeft(sbuf, x1, y, color, text);
            font.drawTextLineRight(sbuf, x2, y, color, v);
            break;
        }
        case MenuOptionType::Input: {
            std::string v = asInput().value;
            font.drawTextLineLeft(sbuf, x1, y, color, text);
            font.drawTextLineRight(sbuf, x2, y, color, v);
            break;
        }
        case MenuOptionType::Spinner: {
            std::string v = std::to_string(asSpinner().value);
            font.drawTextLineLeft(sbuf, x1, y, color, text);
            font.drawTextLineRight(sbuf, x2, y, color, v);
            break;
        }
    }
    dirty = false;
}

Menu::Menu(MenuHandler& handler) : handler_(handler) {
    font_.setFont(getAssets().gameFont);
}

Menu::Menu(Menu&& move) noexcept
    : index_(move.index_),
      init_(move.init_),
      exiting_(move.exiting_),
      handler_(move.handler_),
      font_(std::move(move.font_)),
      options_(std::move(move.options_)) {}

Menu& Menu::operator=(Menu&& move) noexcept {
    index_ = move.index_;
    init_ = move.init_;
    exiting_ = move.exiting_;
    handler_ = move.handler_;
    font_ = std::move(move.font_);
    options_ = std::move(move.options_);
    return *this;
}

void Menu::goUp() {
    int i = index_;
    if (i < 0) i = 0;
    do {
        --i;
        if (i < 0) {
            if (index_ < 0) {
                i = index_;
                break;
            }
            i += static_cast<int>(options_.size());
        }
        if (options_[i].enabled) {
            options_[i].dirty = true;
            break;
        }
    } while (i != index_);

    if (i < 0) return;

    if (i != index_) {
        options_[index_].dirty = true;
        options_[i].dirty = true;
        index_ = i;
    } else if (!options_[index_].enabled) {
        options_[index_].dirty = true;
        index_ = -1;
    }
}

void Menu::goDown() {
    int i = index_;
    if (i < 0) i = 0;
    do {
        ++i;
        if (i >= static_cast<int>(options_.size())) {
            if (index_ < 0) {
                i = index_;
                break;
            }
            i = 0;
        }
        if (options_[i].enabled) {
            options_[i].dirty = true;
            break;
        }
    } while (i != index_);

    if (i < 0) return;

    if (i != index_) {
        options_[index_].dirty = true;
        options_[i].dirty = true;
        index_ = i;
    } else if (!options_[index_].enabled) {
        options_[index_].dirty = true;
        index_ = -1;
    }
}

void Menu::doLeft() {
    if (index_ < 0) {
        pageLeft();
        return;
    }
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select) {
        MenuOptionSelect& sel = option.asSelect();
        if (--sel.index < 0) sel.index += static_cast<int>(sel.options.size());
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    } else if (option.type == MenuOptionType::Spinner) {
        MenuOptionSpinner& spin = option.asSpinner();
        if (spin.value > spin.min) --spin.value;
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    } else {
        pageLeft();
    }
}

void Menu::doRight() {
    if (index_ < 0) {
        pageRight(false);
        return;
    }
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select) {
        MenuOptionSelect& sel = option.asSelect();
        if (++sel.index >= static_cast<int>(sel.options.size())) sel.index = 0;
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    } else if (option.type == MenuOptionType::Spinner) {
        MenuOptionSpinner& spin = option.asSpinner();
        if (spin.value < spin.max) ++spin.value;
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    } else {
        pageRight(false);
    }
}

void Menu::setActiveItem(symbol_t item) {
    int i = -1;
    options_[index_].dirty = true;

    for (std::size_t j = 0; j < options_.size(); ++j) {
        if (options_[j].id == item) {
            i = static_cast<int>(j);
            break;
        }
    }

    index_ = i;
    if (index_ >= 0) options_[index_].dirty = true;
}

void Menu::doSelect() {
    if (info().arcade) {
        exiting_ = exitable_;
        return;
    }
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select ||
        option.type == MenuOptionType::Spinner) {
        doRight();
        return;
    }
    select(index_, option.id);
    sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
}

void Menu::doConfirm(bool yes) {
    confirmId_ = symbol_none;
    std::function<void()>& f = yes ? onMenuYes_.value() : onMenuNo_.value();
    if (f) f();
    onMenuYes_.reset();
    onMenuNo_.reset();
}

void Menu::gotMenuMessage(const MenuMessage& msg) {
    if (!init_) return;
    switch (msg.type) {
        case MenuMessageType::MenuUp:
            goUp();
            if (index_ >= 0)
                sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            break;
        case MenuMessageType::MenuDown:
            goDown();
            if (index_ >= 0)
                sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            break;
        case MenuMessageType::MenuLeft:
            doLeft();
            break;
        case MenuMessageType::MenuRight:
            doRight();
            break;
        case MenuMessageType::MenuSelect:
            if (index_ >= 0 || info().arcade)
                doSelect();
            else
                pageRight(true);
            break;
        case MenuMessageType::MenuExit:
            exiting_ = exitable_;
            break;
        case MenuMessageType::MenuYes:
            dynamic_assert(confirmId_ == msg.menuId && onMenuYes_.has_value(),
                           "ghost message");
            doConfirm(true);
            break;
        case MenuMessageType::MenuNo:
            dynamic_assert(confirmId_ == msg.menuId && onMenuNo_.has_value(),
                           "ghost message");
            doConfirm(false);
            break;
    }
}

static Color getMenuOptionColor(const MenuOption& o, bool selected) {
    return o.type == MenuOptionType::Text || o.enabled
               ? (selected ? menuItemSelectedColor : menuItemColor)
               : menuItemDisabledColor;
}

void Menu::confirm(symbol_t id, const std::string& title,
                   const std::function<void()>& onYes,
                   const std::function<void()>& onNo) {
    dynamic_assert(confirmId_ == symbol_none, "nested confirm not OK!");
    onMenuYes_ = onYes;
    onMenuNo_ = onNo;
    confirmId_ = id;
    openMenu<MenuYesNo>(id, title);
}

coord_t Menu::getMenuOptionY(size_t index) const {
    return (-font_.lineHeight() * static_cast<double>(options_.size() - 1)) /
               2 +
           index * font_.lineHeight();
}

void Menu::runMenu(GameState& state, float interval) {
    if (timeout()) timedOut();
    if (exiting_) {
        auto s = replaceWith_
                     ? handler_.get().replaceMenu(std::move(replaceWith_))
                     : handler_.get().closeMenu();
        if (init_) end(state);
        init_ = false;
        return;
    }
    if (!init_) {
        std::string t = title();
        init_ = true;
        begin(state);
        if (options_.empty()) options_.push_back(MenuOption::spacer());
        if (defaultItem == symbol_none) {
            index_ = 0;
        } else {
            setActiveItem(defaultItem);
            if (index_ < 0) index_ = 0;
        }
        if (!options_[index_].enabled) goDown();

        font_.renderTextLine(titlebuf_, -font_.getTextWidth(t) / 2, -0.875,
                             menuTitleColor, t);
    }
    if (maxLifetime_ != 0) lifetime_ += interval;

    renderSpecial(state.sbuf, interval);
    coord_t x1 = -0.75;
    coord_t x2 = 0.75;
    for (size_t i = 0, e = options_.size(); i < e; ++i) {
        MenuOption& option = options_[i];
        if (option.dirty) {
            option.redraw(x1, x2, getMenuOptionY(i), font_,
                          getMenuOptionColor(option, i == index_));
        }
        state.sbuf.append(option.sbuf);
    }
    state.sbuf.append(titlebuf_);
}

MenuHandler::MenuHandler(const MenuInformation& info) : info_(info) {}

void MenuHandler::gotMessage(const MenuMessage& msg) {
    if (!menus_.empty()) menus_.top()->gotMenuMessage(msg);
}

std::shared_ptr<Menu> MenuHandler::closeMenu() {
    auto s = menus_.top();
    menus_.pop();
    if (!menus_.empty()) menus_.top()->checkTimeout();
    return s;
}

bool MenuHandler::run(GameState& state, float interval) {
    if (!menus_.empty()) menus_.top()->runMenu(state, interval);
    return !menus_.empty();
}

void MenuHandler::closeAllMenus() {
    while (!menus_.empty()) menus_.pop();
}

}  // namespace hiemalia
