/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/explode.cc: implementation of Explosion

#include "game/explode.hh"

#include <cmath>
#include <random>

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
static std::random_device random_device;
static std::default_random_engine random_engine{random_device()};
static std::uniform_real_distribution<coord_t> rd_negative(-0.5, 0.0);
static std::uniform_real_distribution<coord_t> rd_positive(0.0, 0.5);
static std::uniform_real_distribution<coord_t> rd_both(-0.5, 0.5);
static std::uniform_real_distribution<coord_t> rd_speed(0.25, 0.5);
static std::uniform_real_distribution<coord_t> rd_rotation(-6.0, 6.0);

static std::uniform_real_distribution<coord_t>& pick_distribution(coord_t m) {
    if (m < 0)
        return rd_negative;
    else if (m > 0)
        return rd_positive;
    else  // m == 0
        return rd_both;
}

static ModelPoint getRandomVelocity(coord_t xm, coord_t ym, coord_t zm) {
    int i = 0;
    coord_t x, y, z;
    auto& xr = pick_distribution(xm);
    auto& yr = pick_distribution(ym);
    auto& zr = pick_distribution(zm);
    do {
        x = xr(random_engine);
        y = yr(random_engine);
        z = zr(random_engine);
    } while (x == 0 && y == 0 && z == 0 && i++ < 50);

    if (!(x == 0 && y == 0 && z == 0)) {
        coord_t v = 1.0 / std::sqrt(x * x + y * y + z * z);
        x *= rd_speed(random_engine) * v;
        y *= rd_speed(random_engine) * v;
        z *= rd_speed(random_engine) * v;
    }
    return ModelPoint(x, y, z);
}

static Rotation3D getRandomRotation(coord_t xm, coord_t ym, coord_t zm) {
    return Rotation3D{rd_rotation(random_engine), rd_rotation(random_engine),
                      rd_rotation(random_engine)};
}

Explosion::Explosion(const GameObject& o, coord_t xm, coord_t ym, coord_t zm)
    : GameObject() {
    tempModel.vertices.push_back(ModelPoint(0, 0, 0));
    tempModel.vertices.push_back(ModelPoint(0, 0, 0));
    tempModel.shapes.push_back(ModelFragment(Color{255, 128, 0, 255}, 0, {1}));
    pos = o.pos;
    rot = o.rot;
    scale = o.scale;
    const Model& model = *o.model;
    for (const ModelFragment& f : model.shapes) {
        const ModelPoint* prev = &(model.vertices[f.start]);
        for (size_t pi : f.points) {
            const ModelPoint& p0 = *prev;
            const ModelPoint& p1 = model.vertices[pi];
            ModelPoint c = ModelPoint::average(p0, p1);
            shards.push_back({p0 - c, p1 - c, pos + c, rot,
                              getRandomVelocity(xm, ym, zm),
                              getRandomRotation(xm, ym, zm)});
            prev = &p1;
        }
    }
}

bool Explosion::update(GameWorld& w, float delta) {
    alpha -= delta * 0.4;
    coord_t mul = std::pow(0.5, 1.0 / delta);
    for (ExplosionShard& shard : shards) {
        shard.pos += shard.dpos * delta;
        shard.rot += shard.drot * delta;
        shard.drot *= mul;
    }
    tempModel.shapes[0].color.a = static_cast<uint8_t>(255 * std::sqrt(alpha));
    return alpha > 0;
}

void Explosion::renderSpecial(SplinterBuffer& sbuf, Renderer3D& r3d) {
    for (ExplosionShard& shard : shards) {
        tempModel.vertices[0] = shard.p0;
        tempModel.vertices[1] = shard.p1;
        r3d.renderModel(sbuf, shard.pos, shard.rot, scale, tempModel);
    }
}

void Explosion::adjustSpeed(coord_t s) {
    s *= 4;
    for (ExplosionShard& shard : shards) {
        shard.dpos *= s;
        shard.drot *= s;
    }
}

}  // namespace hiemalia
