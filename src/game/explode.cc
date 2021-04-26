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
static std::uniform_real_distribution<coord_t> rd_rotation(-48.0, 48.0);
static std::uniform_int_distribution<int> rd_split(0, 3);

static std::uniform_real_distribution<coord_t>& pick_distribution(coord_t m) {
    if (m < 0)
        return rd_negative;
    else if (m > 0)
        return rd_positive;
    else  // m == 0
        return rd_both;
}

static Point3D getRandomVelocity(coord_t xm, coord_t ym, coord_t zm) {
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
    return Point3D(x, y, z);
}

static Orient3D getRandomRotation(coord_t xm, coord_t ym, coord_t zm) {
    return Orient3D{random(rd_rotation), random(rd_rotation),
                    random(rd_rotation)};
}

void Explosion::cutShards(coord_t xm, coord_t ym, coord_t zm) {
    float explspeedinv = 1.0f / explspeed_;
    for (int i = 0, e = shards_p0_.size(); i < e; ++i) {
        if ((shards_p1_[i] - shards_p0_[i]).lengthSquared() > 0.015625 &&
            random(rd_split) != 0) {
            shards_p0_[i] *= 0.5;
            shards_p1_[i] *= 0.5;
            shards_p0_.push_back(shards_p0_[i]);
            shards_p1_.push_back(shards_p1_[i]);
            shards_pos_.push_back(pos + shards_p0_[i]);
            shards_dpos_.push_back(getRandomVelocity(xm, ym, zm) *
                                   explspeedinv);
            shards_rot_.push_back(rot);
            shards_drot_.push_back(getRandomRotation(xm, ym, zm));
            shards_pos_[i] += shards_p1_[i];
        }
    }
}

Explosion::Explosion(const Point3D& pos, const GameObject& o, coord_t xm,
                     coord_t ym, coord_t zm, float explspeed)
    : Explosion(pos, o, o.model(), xm, ym, zm, explspeed) {}

Explosion::Explosion(const Point3D& p, const GameObject& o, const Model& model,
                     coord_t xm, coord_t ym, coord_t zm, float explspeed)
    : GameObject(p), explspeed_(explspeed) {
    float explspeedinv = 1.0f / explspeed;
    tempModel_.vertices.push_back(Point3D(0, 0, 0));
    tempModel_.vertices.push_back(Point3D(0, 0, 0));
    tempModel_.shapes.push_back(ModelFragment(Color{255, 128, 0, 255}, 0, {1}));
    rot = o.rot;
    for (const ModelFragment& f : model.shapes) {
        const Point3D* prev = &(model.vertices[f.start]);
        for (size_t pi : f.points) {
            Point3D p0 = prev->hadamard(o.scale);
            Point3D p1 = model.vertices[pi].hadamard(o.scale);
            Point3D c = Point3D::average(p0, p1);
            shards_p0_.push_back(p0 - c);
            shards_p1_.push_back(p1 - c);
            shards_pos_.push_back(pos + c);
            shards_dpos_.push_back(getRandomVelocity(xm, ym, zm) *
                                   explspeedinv);
            shards_rot_.push_back(rot);
            shards_drot_.push_back(getRandomRotation(xm, ym, zm));
            prev = &p1;
        }
    }

    for (int i = 0; i < 3 && shards_p0_.size() <= maxShards / 2; ++i)
        cutShards(xm, ym, zm);
}

bool Explosion::update(GameWorld& w, float delta) {
    alpha_ -= delta * 0.4f * explspeed_;
    // coord_t mul = std::pow(0.5, 1.0 / delta);
    for (int i = 0, e = shards_pos_.size(); i < e; ++i) {
        shards_pos_[i] += shards_dpos_[i] * delta;
        shards_rot_[i] += shards_drot_[i] * delta;
        // shards_drot_[i] *= mul;
    }
    tempModel_.shapes[0].color.a =
        static_cast<uint8_t>(255 * std::sqrt(alpha_));
    return alpha_ > 0;
}

void Explosion::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    for (int i = 0, e = shards_pos_.size(); i < e; ++i) {
        tempModel_.vertices[0] = shards_p0_[i];
        tempModel_.vertices[1] = shards_p1_[i];
        r3d.renderModel(sbuf, shards_pos_[i], shards_rot_[i], scale,
                        tempModel_);
    }
}

void Explosion::adjustSpeed(coord_t s) {
    for (int i = 0, e = shards_pos_.size(); i < e; ++i) {
        shards_dpos_[i] *= s;
        shards_drot_[i] *= s;
    }
}

void Explosion::onMove(const Point3D& p) {
    const Point3D& d = p - pos;
    for (int i = 0, e = shards_pos_.size(); i < e; ++i) {
        shards_pos_[i] += d;
    }
}

}  // namespace hiemalia
