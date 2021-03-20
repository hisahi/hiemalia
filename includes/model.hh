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
};

struct ModelFragment {
    Color color;
    ModelPoint start;
    std::vector<ModelPoint> points;

    ModelFragment(const Color& color, ModelPoint start)
        : color(color), start(start), points() {}
    ModelFragment(const Color& color, ModelPoint start,
                  std::initializer_list<ModelPoint> points)
        : color(color), start(start), points(points) {}
    ModelFragment(const Color& color, ModelPoint start,
                  std::vector<ModelPoint>&& points)
        : color(color), start(start), points(std::move(points)) {}
};

struct Model {
    std::vector<ModelFragment> parts;

    Model() : parts() {}
    Model(std::vector<ModelFragment>&& parts) : parts(std::move(parts)) {}
};

};  // namespace hiemalia

#endif  // M_MODEL_HH
