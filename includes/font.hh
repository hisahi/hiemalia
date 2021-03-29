/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// font.hh: header file for fonts

#ifndef M_FONT_HH
#define M_FONT_HH

#include "defs.hh"
#include "shape.hh"

namespace hiemalia {
struct Font {
    ShapeSheet shapes;
    coord_t width;
    coord_t height;
    char minChar;
    char maxChar;

    Font(ShapeSheet shapes, coord_t width, coord_t height, char minChar)
        : shapes(shapes),
          width(width),
          height(height),
          minChar(minChar),
          maxChar(static_cast<char>(minChar + shapes.count())) {}
};
}  // namespace hiemalia

#endif  // M_FONT_HH
