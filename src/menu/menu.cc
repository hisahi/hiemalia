/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu.cc: implementation of Menu

#include "menu.hh"

#include "defs.hh"
#include "hiemalia.hh"
#include "load2d.hh"
#include "logger.hh"
#include "logic.hh"

namespace hiemalia {

static const Color menuItemSelectedColor{255, 255, 255, 255};
static const Color menuItemColor{255, 255, 255, 160};

void MenuOption::redraw(coord_t x1, coord_t x2, coord_t y,
                        const RendererText& font, const Color& color) {
    sbuf.clear();
    switch (type) {
        case MenuOptionType::Button:
            font.renderTextLine(sbuf, (x1 + x2 - font.getTextWidth(text)) / 2,
                                y, color, text);
            break;
        case MenuOptionType::Text:
            font.renderTextLine(sbuf, x1, y, color, text);
            break;
        case MenuOptionType::Select:
            font.renderTextLine(sbuf, x1, y, color, text);
            break;
    }
    dirty = false;
}

Menu::Menu(std::shared_ptr<Menu>&& parent) : parent_(std::move(parent)) {
    font_.setFont(std::move(loadFont("font.2d")));
}

Menu::Menu(Menu&& move) noexcept
    : LogicModule(std::move(move)),
      index_(move.index_),
      init_(move.init_),
      exiting_(move.exiting_),
      parent_(move.parent_),
      font_(std::move(move.font_)),
      options_(std::move(move.options_)) {}

Menu& Menu::operator=(Menu&& move) noexcept {
    LogicModule::operator=(std::move(move));
    index_ = move.index_;
    init_ = move.init_;
    exiting_ = move.exiting_;
    parent_ = move.parent_;
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

void Menu::doLeft() {}

void Menu::doRight() {}

void Menu::gotMessage(const MenuMessage& msg) {
    if (!init_) return;
    switch (msg.type) {
        case MenuMessageType::MenuUp:
            goUp();
            break;
        case MenuMessageType::MenuDown:
            goDown();
            break;
        case MenuMessageType::MenuLeft:
            doLeft();
            break;
        case MenuMessageType::MenuRight:
            doRight();
            break;
        case MenuMessageType::MenuSelect:
            if (index_ >= 0) select(index_, options_[index_].id);
            break;
        case MenuMessageType::MenuExit:
            if (parent_ != nullptr) exiting_ = true;
            break;
    }
}

bool Menu::run(GameState& state, float interval) {
    if (!init_) {
        init_ = true;
        begin(state);
        dynamic_assert(options_.size() > 0, "menu cannot be empty");
        index_ = 0;
        if (!options_[0].enabled) goDown();
    }
    if (exiting_) {
        end(state);
        sendMessage(LogicMessage::startMenu(std::move(parent_)));
        return false;
    }

    coord_t x1 = -0.75;
    coord_t x2 = 0.75;
    coord_t y = -font_.lineHeight() * (options_.size() - 1);
    for (int i = 0, e = options_.size(); i < e; ++i) {
        MenuOption& option = options_[i];
        if (option.dirty) {
            option.redraw(x1, x2, y, font_,
                          i == index_ ? menuItemSelectedColor : menuItemColor);
        }
        state.sbuf.append(option.sbuf);
        y += font_.lineHeight();
    }

    return true;
}

}  // namespace hiemalia
