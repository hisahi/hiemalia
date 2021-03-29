/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rend2d.hh: header file for rend2d.cc

#ifndef M_REND2D_HH
#define M_REND2D_HH

#include <string>

#include "defs.hh"
#include "sbuf.hh"
#include "shape.hh"

namespace hiemalia {
struct Matrix2D {
    coord_t a;
    coord_t b;
    coord_t c;
    coord_t d;

    Matrix2D() : a(1), b(0), c(0), d(1) {}
};

class Renderer2D {
   public:
    void renderShape(SplinterBuffer& buf, coord_t x, coord_t y,
                     const Shape& s) const;
    void renderShapeColor(SplinterBuffer& buf, coord_t x, coord_t y,
                          const Color& clr, const Shape& s) const;
    coord_t projectX(coord_t x, coord_t y) const {
        return x * _mat.a + y * _mat.b;
    }
    coord_t projectY(coord_t x, coord_t y) const {
        return x * _mat.c + y * _mat.d;
    }
    void setMatrix(coord_t a, coord_t b, coord_t c, coord_t d) {
        _mat.a = a;
        _mat.b = b;
        _mat.c = c;
        _mat.d = d;
    }
    void resetMatrix() { setMatrix(1, 0, 0, 1); }

   private:
    void renderFragment(SplinterBuffer& buf, coord_t x, coord_t y,
                        const Color& clr, const ShapeFragment& f) const;
    Matrix2D _mat;
};
};  // namespace hiemalia

#endif  // M_REND2D_HH
