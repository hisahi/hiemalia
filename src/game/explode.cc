/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/explode.cc: implementation of Explosion

#include "game/explode.hh"

#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"
#include "model.hh"
#include "random.hh"
#include "rend3d.hh"

namespace hiemalia {
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
        x = random(xr);
        y = random(yr);
        z = random(zr);
    } while (x == 0 && y == 0 && z == 0 && i++ < 50);

    if (!(x == 0 && y == 0 && z == 0)) {
        coord_t v = 1.0 / std::sqrt(x * x + y * y + z * z);
        x *= random(rd_speed) * v;
        y *= random(rd_speed) * v;
        z *= random(rd_speed) * v;
    }
    return ModelPoint(x, y, z);
}

static Rotation3D getRandomRotation(coord_t xm, coord_t ym, coord_t zm) {
    return Rotation3D{random(rd_rotation), random(rd_rotation),
                      random(rd_rotation)};
}

Explosion::Explosion(const GameObject& o, coord_t xm, coord_t ym, coord_t zm,
                     float explspeed)
    : Explosion(o, o.model(), xm, ym, zm, explspeed) {}

Explosion::Explosion(const GameObject& o, const Model& model, coord_t xm,
                     coord_t ym, coord_t zm, float explspeed)
    : GameObject(), explspeed_(explspeed) {
    float explspeedinv = 1.0f / explspeed;
    tempModel_.vertices.push_back(ModelPoint(0, 0, 0));
    tempModel_.vertices.push_back(ModelPoint(0, 0, 0));
    tempModel_.shapes.push_back(ModelFragment(Color{255, 128, 0, 255}, 0, {1}));
    pos = o.pos;
    rot = o.rot;
    scale = o.scale;
    for (const ModelFragment& f : model.shapes) {
        const ModelPoint* prev = &(model.vertices[f.start]);
        for (size_t pi : f.points) {
            const ModelPoint& p0 = *prev;
            const ModelPoint& p1 = model.vertices[pi];
            ModelPoint c = ModelPoint::average(p0, p1);
            shards_.push_back({p0 - c, p1 - c, pos + c, rot,
                               getRandomVelocity(xm, ym, zm) * explspeedinv,
                               getRandomRotation(xm, ym, zm)});
            prev = &p1;
        }
    }
}

bool Explosion::update(GameWorld& w, float delta) {
    alpha_ -= delta * 0.4f * explspeed_;
    coord_t mul = std::pow(0.5, 1.0 / delta);
    for (ExplosionShard& shard : shards_) {
        shard.pos += shard.dpos * delta;
        shard.rot += shard.drot * delta;
        shard.drot *= mul;
    }
    tempModel_.shapes[0].color.a =
        static_cast<uint8_t>(255 * std::sqrt(alpha_));
    return alpha_ > 0;
}

void Explosion::renderSpecial(SplinterBuffer& sbuf, Renderer3D& r3d) {
    for (ExplosionShard& shard : shards_) {
        tempModel_.vertices[0] = shard.p0;
        tempModel_.vertices[1] = shard.p1;
        r3d.renderModel(sbuf, shard.pos, shard.rot, scale, tempModel_);
    }
}

void Explosion::adjustSpeed(coord_t s) {
    for (ExplosionShard& shard : shards_) {
        shard.dpos *= s;
        shard.drot *= s;
    }
}

void Explosion::move(const ModelPoint& p) {
    for (ExplosionShard& shard : shards_) {
        shard.pos += p;
    }
}

}  // namespace hiemalia
