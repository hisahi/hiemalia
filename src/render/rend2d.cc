/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rend2d.cc: implementation of Renderer2D

#include "rend2d.hh"

#include "sbuf.hh"
#include "shape.hh"

namespace hiemalia {
void Renderer2D::renderShape(SplinterBuffer& buf, coord_t x, coord_t y,
                             const Shape& s) const {
    for (const ShapeFragment& part : s.parts)
        renderFragment(buf, x, y, part.color, part);
}

void Renderer2D::renderShapeColor(SplinterBuffer& buf, coord_t x, coord_t y,
                                  const Color& clr, const Shape& s) const {
    for (const ShapeFragment& part : s.parts)
        renderFragment(buf, x, y, clr, part);
}

void Renderer2D::renderFragment(SplinterBuffer& buf, coord_t x, coord_t y,
                                const Color& clr,
                                const ShapeFragment& f) const {
    if (f.points.empty()) return;
    buf.push(Splinter{SplinterType::BeginShape,
                      x + projectX(f.start.x, f.start.y),
                      y + projectY(f.start.x, f.start.y), clr});
    for (auto it = f.points.begin(); it != f.points.end(); ++it) {
        const ShapePoint& p = *it;
        buf.push(Splinter(SplinterType::Point, x + projectX(p.x, p.y),
                          y + projectY(p.x, p.y), clr));
    }
    buf.endShape();
}
}  // namespace hiemalia
