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
    friend inline ModelPoint operator*(ModelPoint a, const coord_t& f) {
        return a *= f;
    }
};

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
