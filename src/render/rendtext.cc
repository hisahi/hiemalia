/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rendtext.cc: implementation of RendererText

#include "rendtext.hh"

#include <algorithm>

#include "font.hh"
#include "logger.hh"

namespace hiemalia {
RendererText::RendererText() { setScale(scalex_, scaley_); }

void RendererText::renderCharacter(SplinterBuffer& buf, coord_t x, coord_t y,
                                   const Color& clr, char c) const {
    if (c >= font_->minChar && c < font_->maxChar) {
        Renderer2D::renderShapeColor(buf, x, y, clr,
                                     font_->shapes[c - font_->minChar]);
    }
}

void RendererText::renderTextLine(SplinterBuffer& buf, coord_t x, coord_t y,
                                  const Color& clr, std::string s) const {
    for (const char& c : s) {
        renderCharacter(buf, x, y, clr, c);
        x += font_->width * scalex_;
    }
}

void RendererText::setScale(coord_t scalex, coord_t scaley) {
    scalex_ = scalex;
    scaley_ = scaley;
    Renderer2D::setMatrix(scalex, 0, 0, scaley);
}

coord_t RendererText::getTextWidth(std::string s) const {
    return (s.empty() ? 0 : s.length() - 1) * font_->width * scalex_;
}

void RendererText::renderText(SplinterBuffer& buf, coord_t x, coord_t y,
                              const Color& clr, std::string s) const {
    coord_t ox = x;
    for (const char& c : s) {
        if (c == '\n') {
            y += lineHeight();
            x = ox;
            continue;
        }
        renderCharacter(buf, x, y, clr, c);
        x += font_->width * scalex_;
    }
}

void RendererText::drawTextLineLeft(SplinterBuffer& buf, coord_t x, coord_t y,
                                    const Color& clr, std::string s,
                                    coord_t scale) {
    if (scale != 1) {
        coord_t sx = scalex_, sy = scaley_;
        setScale(sx * scale, sy * scale);
        renderTextLine(buf, x, y, clr, s);
        setScale(sx, sy);
    } else {
        renderTextLine(buf, x, y, clr, s);
    }
}

void RendererText::drawTextLineCenter(SplinterBuffer& buf, coord_t x, coord_t y,
                                      const Color& clr, std::string s,
                                      coord_t scale) {
    if (scale != 1) {
        coord_t sx = scalex_, sy = scaley_;
        setScale(sx * scale, sy * scale);
        x -= getTextWidth(s) / 2;
        renderTextLine(buf, x, y, clr, s);
        setScale(sx, sy);
    } else {
        x -= getTextWidth(s) / 2;
        renderTextLine(buf, x, y, clr, s);
    }
}

void RendererText::drawTextLineRight(SplinterBuffer& buf, coord_t x, coord_t y,
                                     const Color& clr, std::string s,
                                     coord_t scale) {
    if (scale != 1) {
        coord_t sx = scalex_, sy = scaley_;
        setScale(sx * scale, sy * scale);
        x -= getTextWidth(s);
        renderTextLine(buf, x, y, clr, s);
        setScale(sx, sy);
    } else {
        x -= getTextWidth(s);
        renderTextLine(buf, x, y, clr, s);
    }
}
}  // namespace hiemalia
