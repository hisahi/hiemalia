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
#include "math.hh"
#include "sbuf.hh"

namespace hiemalia {
struct Point2D {
    coord_t x;
    coord_t y;

    Point2D(coord_t x, coord_t y) : x(x), y(y) {}

    inline Point2D operator-() const { return Point2D(-x, -y); }

    inline Point2D& operator+=(const Point2D& r) noexcept {
        x += r.x;
        y += r.y;
        return *this;
    }
    inline Point2D& operator-=(const Point2D& r) noexcept {
        x -= r.x;
        y -= r.y;
        return *this;
    }
    inline Point2D& operator*=(const coord_t& f) noexcept {
        x *= f;
        y *= f;
        return *this;
    }

    friend inline Point2D operator+(Point2D a, const Point2D& b) noexcept {
        return a += b;
    }
    friend inline Point2D operator-(Point2D a, const Point2D& b) noexcept {
        return a -= b;
    }
    friend inline Point2D operator*(const coord_t& f, Point2D a) noexcept {
        return a *= f;
    }
    friend inline Point2D operator*(Point2D a, const coord_t& f) noexcept {
        return a *= f;
    }

    inline bool operator==(const Point2D& b) const noexcept {
        return x == b.x && y == b.y;
    }
    inline bool operator!=(const Point2D& b) const noexcept {
        return !(*this == b);
    }

    inline coord_t lengthSquared() const noexcept { return x * x + y * y; }
    inline coord_t length() const noexcept { return sqrt(lengthSquared()); }
    inline Point2D normalize() const {
        dynamic_assert(x != 0 || y != 0, "attempt to normalize zero vector!");
        return *this * (1 / sqrt(lengthSquared()));
    }
};

struct ShapeFragment {
    Color color;
    Point2D start;
    std::vector<Point2D> points;

    ShapeFragment(const Color& color, Point2D start)
        : color(color), start(start), points() {}
    ShapeFragment(const Color& color, Point2D start,
                  std::vector<Point2D>&& points) noexcept
        : color(color), start(start), points(std::move(points)) {}
};

struct Shape {
    std::vector<ShapeFragment> parts;

    Shape() : parts() {}
    Shape(std::vector<ShapeFragment>&& parts) noexcept
        : parts(std::move(parts)) {}
};

struct ShapeSheet {
    std::vector<Shape> shapes;

    inline Shape& operator[](size_t index) { return shapes[index]; }
    inline const Shape& operator[](size_t index) const { return shapes[index]; }
    inline size_t count() const noexcept { return shapes.size(); }

    ShapeSheet() : shapes() {}
    ShapeSheet(std::vector<Shape>&& shapes) noexcept
        : shapes(std::move(shapes)) {}
};

};  // namespace hiemalia

#endif  // M_SHAPE_HH
