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
#include "load2d.hh"
#include "logger.hh"
#include "logic.hh"
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
            i += options_.size();
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
        if (--sel.index < 0) sel.index += sel.options.size();
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
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select) {
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
            if (index_ >= 0)
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
                   std::function<void()> onYes, std::function<void()> onNo) {
    dynamic_assert(confirmId_ == symbol_none, "nested confirm not OK!");
    onMenuYes_ = onYes;
    onMenuNo_ = onNo;
    confirmId_ = id;
    openMenu<MenuYesNo>(id, title);
}

coord_t Menu::getMenuOptionY(int index) const {
    return (-font_.lineHeight() * (options_.size() - 1)) / 2 +
           index * font_.lineHeight();
}

void Menu::runMenu(GameState& state, float interval) {
    if (exiting_) {
        auto s = handler_.get().closeMenu();
        if (init_) end(state);
        init_ = false;
        return;
    }
    if (!init_) {
        std::string t = title();
        init_ = true;
        begin(state);
        dynamic_assert(options_.size() > 0, "menu cannot be empty");
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

    renderSpecial(state.sbuf, interval);
    coord_t x1 = -0.75;
    coord_t x2 = 0.75;
    for (int i = 0, e = options_.size(); i < e; ++i) {
        MenuOption& option = options_[i];
        if (option.dirty) {
            option.redraw(x1, x2, getMenuOptionY(i), font_,
                          getMenuOptionColor(option, i == index_));
        }
        state.sbuf.append(option.sbuf);
    }
    state.sbuf.append(titlebuf_);
}

MenuHandler::MenuHandler() {}

void MenuHandler::gotMessage(const MenuMessage& msg) {
    if (!menus_.empty()) menus_.top()->gotMenuMessage(msg);
}

bool MenuHandler::run(GameState& state, float interval) {
    if (!menus_.empty()) menus_.top()->runMenu(state, interval);
    return !menus_.empty();
}

}  // namespace hiemalia
