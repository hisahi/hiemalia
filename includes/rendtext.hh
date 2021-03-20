/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rendtext.hh: header file for rendtext.cc

#ifndef M_RENDTEXT_HH
#define M_RENDTEXT_HH

#include <string>

#include "defs.hh"
#include "rend2d.hh"
#include "sbuf.hh"
#include "shape.hh"

namespace hiemalia {
struct RendererTextFont {
    ShapeSheet shapes;
    coord_t width;
    coord_t height;
    char minChar;
    char maxChar;

    RendererTextFont(ShapeSheet shapes, coord_t width, coord_t height,
                     char minChar)
        : shapes(shapes),
          width(width),
          height(height),
          minChar(minChar),
          maxChar(minChar + shapes.count()) {}
};

class RendererText : private Renderer2D {
   public:
    RendererText();
    void renderCharacter(SplinterBuffer& buf, coord_t x, coord_t y,
                         const Color& clr, char c) const;
    void renderTextLine(SplinterBuffer& buf, coord_t x, coord_t y,
                        const Color& clr, std::string s) const;
    void renderText(SplinterBuffer& buf, coord_t x, coord_t y, const Color& clr,
                    std::string s) const;
    coord_t getTextWidth(std::string s) const;
    void setFont(const RendererTextFont& font) { font_ = font; }
    void setFont(RendererTextFont&& font) { font_ = std::move(font); }
    coord_t scaleX() const { return scalex_; }
    coord_t scaleY() const { return scaley_; }
    coord_t lineHeight() const { return scaley_ * font_.height; }
    void setScale(coord_t scalex, coord_t scaley);

   private:
    RendererTextFont font_{{}, 0, 0, 0};
    coord_t scalex_{1.0 / 40};
    coord_t scaley_{1.0 / 30};
};
};  // namespace hiemalia

#endif  // M_RENDTEXT_HH
