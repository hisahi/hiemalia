/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rendtext.hh: header file for rendtext.cc

#ifndef M_RENDTEXT_HH
#define M_RENDTEXT_HH

#include <memory>
#include <string>

#include "defs.hh"
#include "font.hh"
#include "rend2d.hh"
#include "sbuf.hh"

namespace hiemalia {
class RendererText : private Renderer2D {
  public:
    RendererText();
    void renderCharacter(SplinterBuffer& buf, coord_t x, coord_t y,
                         const Color& clr, char c) const;
    void renderTextLine(SplinterBuffer& buf, coord_t x, coord_t y,
                        const Color& clr, std::string s) const;
    void renderText(SplinterBuffer& buf, coord_t x, coord_t y, const Color& clr,
                    std::string s) const;
    void drawTextLineLeft(SplinterBuffer& buf, coord_t x, coord_t y,
                          const Color& clr, std::string s, coord_t scale = 1);
    void drawTextLineCenter(SplinterBuffer& buf, coord_t x, coord_t y,
                            const Color& clr, std::string s, coord_t scale = 1);
    void drawTextLineRight(SplinterBuffer& buf, coord_t x, coord_t y,
                           const Color& clr, std::string s, coord_t scale = 1);
    coord_t getTextWidth(std::string s) const;
    inline void setFont(const std::shared_ptr<Font>& font) { font_ = font; }
    inline coord_t scaleX() const { return scalex_; }
    inline coord_t scaleY() const { return scaley_; }
    inline coord_t charWidth() const { return scalex_ * font_->width; }
    inline coord_t lineHeight() const { return scaley_ * font_->height; }
    void setScale(coord_t scalex, coord_t scaley);

  private:
    std::shared_ptr<Font> font_;
    coord_t scalex_{1.0 / 40};
    coord_t scaley_{1.0 / 30};
};
};  // namespace hiemalia

#endif  // M_RENDTEXT_HH
