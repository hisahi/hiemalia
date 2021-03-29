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
#include "sounds.hh"

namespace hiemalia {

static const Color menuTitleColor{255, 255, 255, 255};
static const Color menuItemSelectedColor{255, 255, 255, 255};
static const Color menuItemColor{255, 255, 255, 160};
static const Color menuItemDisabledColor{255, 255, 255, 96};

void MenuOption::redraw(coord_t x1, coord_t x2, coord_t y,
                        const RendererText& font, const Color& color) {
    sbuf.clear();
    switch (type) {
        case MenuOptionType::Spacer:
            break;
        case MenuOptionType::Text:
            font.renderTextLine(sbuf, x1, y, color, text);
            break;
        case MenuOptionType::Button:
            font.renderTextLine(sbuf, (x1 + x2 - font.getTextWidth(text)) / 2,
                                y, color, text);
            break;
        case MenuOptionType::Select: {
            std::string v = asSelect().options.at(asSelect().index);
            font.renderTextLine(sbuf, x1, y, color, text);
            font.renderTextLine(sbuf, x2 - font.getTextWidth(v), y, color, v);
            break;
        }
        case MenuOptionType::Input: {
            std::string v = asInput().value;
            font.renderTextLine(sbuf, x1, y, color, text);
            font.renderTextLine(sbuf, x2 - font.getTextWidth(v), y, color, v);
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
        if (i < 0) i += options_.size();
        if (options_[i].enabled) {
            options_[i].dirty = true;
            break;
        }
    } while (i != index_);

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
        if (i >= static_cast<int>(options_.size())) i = 0;
        if (options_[i].enabled) {
            options_[i].dirty = true;
            break;
        }
    } while (i != index_);

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
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select) {
        MenuOptionSelect& sel = option.asSelect();
        if (--sel.index < 0) sel.index += sel.options.size();
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    }
}

void Menu::doRight() {
    MenuOption& option = options_[index_];
    if (option.type == MenuOptionType::Select) {
        MenuOptionSelect& sel = option.asSelect();
        if (++sel.index >= static_cast<int>(sel.options.size())) sel.index = 0;
        option.dirty = true;
        select(index_, option.id);
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    }
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

void Menu::gotMenuMessage(const MenuMessage& msg) {
    if (!init_) return;
    switch (msg.type) {
        case MenuMessageType::MenuUp:
            goUp();
            sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            break;
        case MenuMessageType::MenuDown:
            goDown();
            sendMessage(AudioMessage::playSound(SoundEffect::MenuChange));
            break;
        case MenuMessageType::MenuLeft:
            doLeft();
            break;
        case MenuMessageType::MenuRight:
            doRight();
            break;
        case MenuMessageType::MenuSelect:
            if (index_ >= 0) doSelect();
            break;
        case MenuMessageType::MenuExit:
            exiting_ = exitable_;
            break;
    }
}

static Color getMenuOptionColor(const MenuOption& o, bool selected) {
    return o.type == MenuOptionType::Text || o.enabled
               ? (selected ? menuItemSelectedColor : menuItemColor)
               : menuItemDisabledColor;
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
        index_ = 0;
        if (!options_[0].enabled) goDown();
        font_.renderTextLine(titlebuf_, -font_.getTextWidth(t) / 2, -0.875,
                             menuTitleColor, t);
    }

    specialRender(state.sbuf, interval);
    coord_t x1 = -0.75;
    coord_t x2 = 0.75;
    coord_t y = (-font_.lineHeight() * (options_.size() - 1)) / 2;
    for (int i = 0, e = options_.size(); i < e; ++i) {
        MenuOption& option = options_[i];
        if (option.dirty) {
            option.redraw(x1, x2, y, font_,
                          getMenuOptionColor(option, i == index_));
        }
        state.sbuf.append(option.sbuf);
        y += font_.lineHeight();
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
