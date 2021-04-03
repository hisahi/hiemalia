/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// model.hh: header file for 3D models

#ifndef M_MODEL_HH
#define M_MODEL_HH

#include <vector>

#include "defs.hh"
#include "math.hh"
#include "sbuf.hh"

namespace hiemalia {
struct ModelPoint {
    coord_t x;
    coord_t y;
    coord_t z;

    ModelPoint(coord_t x, coord_t y, coord_t z) : x(x), y(y), z(z) {}

    inline ModelPoint operator-() const { return ModelPoint(-x, -y, -z); }

    inline ModelPoint& operator+=(const ModelPoint& r) {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    inline ModelPoint& operator-=(const ModelPoint& r) {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    inline ModelPoint& operator*=(const coord_t& f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    friend inline ModelPoint operator+(ModelPoint a, const ModelPoint& b) {
        return a += b;
    }
    friend inline ModelPoint operator-(ModelPoint a, const ModelPoint& b) {
        return a -= b;
    }
    friend inline ModelPoint operator*(const coord_t& f, ModelPoint a) {
        return a *= f;
    }
    friend inline ModelPoint operator*(ModelPoint a, const coord_t& f) {
        return a *= f;
    }

    inline static ModelPoint average(const ModelPoint& a, const ModelPoint& b) {
        return ModelPoint(0.5 * (a.x + b.x), 0.5 * (a.y + b.y),
                          0.5 * (a.z + b.z));
    }
    inline coord_t lengthSquared() { return x * x + y * y + z * z; }
    inline coord_t length() { return sqrt(lengthSquared()); }
    inline ModelPoint normalize() {
        dynamic_assert(x != 0 || y != 0 || z != 0,
                       "attempt to normalize zero vector!");
        return *this * (1 / sqrt(lengthSquared()));
    }
};

inline ModelPoint lerpPoint(const ModelPoint& a, coord_t t,
                            const ModelPoint& b) {
    return ModelPoint(lerp(a.x, t, b.x), lerp(a.y, t, b.y), lerp(a.z, t, b.z));
}

inline ModelPoint sqerpPoint(const ModelPoint& a, coord_t t,
                             const ModelPoint& b) {
    return ModelPoint(sqerp(a.x, t, b.x), sqerp(a.y, t, b.y),
                      sqerp(a.z, t, b.z));
}

struct ModelFragment {
    Color color;
    size_t start;
    std::vector<size_t> points;

    ModelFragment(const Color& color, size_t start)
        : color(color), start(start), points() {}
    ModelFragment(const Color& color, size_t start,
                  std::initializer_list<size_t> points)
        : color(color), start(start), points(points) {}
    ModelFragment(const Color& color, size_t start,
                  std::vector<size_t>&& points)
        : color(color), start(start), points(std::move(points)) {}
};

struct Model {
    std::vector<ModelPoint> vertices;
    std::vector<ModelFragment> shapes;

    Model() : vertices(), shapes() {}
    Model(std::vector<ModelPoint>&& vertices,
          std::vector<ModelFragment>&& shapes)
        : vertices(std::move(vertices)), shapes(std::move(shapes)) {}
};

};  // namespace hiemalia

#endif  // M_MODEL_HH
