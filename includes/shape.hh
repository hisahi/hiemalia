/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// shape.hh: header file for shapes

#ifndef M_SHAPE_HH
#define M_SHAPE_HH

#include <vector>

#include "defs.hh"
#include "sbuf.hh"

namespace hiemalia {
struct ShapePoint {
    coord_t x;
    coord_t y;

    ShapePoint(coord_t x, coord_t y) : x(x), y(y) {}
};

struct ShapeFragment {
    Color color;
    ShapePoint start;
    std::vector<ShapePoint> points;

    ShapeFragment(const Color& color, ShapePoint start)
        : color(color), start(start), points() {}
    ShapeFragment(const Color& color, ShapePoint start,
                  std::vector<ShapePoint>&& points)
        : color(color), start(start), points(std::move(points)) {}
};

struct Shape {
    std::vector<ShapeFragment> parts;

    Shape() : parts() {}
    Shape(std::vector<ShapeFragment>&& parts) : parts(std::move(parts)) {}
};

struct ShapeSheet {
    std::vector<Shape> shapes;

    inline Shape& operator[](size_t index) { return shapes[index]; }
    inline const Shape& operator[](size_t index) const { return shapes[index]; }
    inline size_t count() { return shapes.size(); }

    ShapeSheet() : shapes() {}
    ShapeSheet(std::vector<Shape>&& shapes) : shapes(std::move(shapes)) {}
};

};  // namespace hiemalia

#endif  // M_SHAPE_HH
