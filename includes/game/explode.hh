/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/explode.hh: header file for the explosion object

#ifndef M_GAME_EXPLODE_HH
#define M_GAME_EXPLODE_HH

#include "assets.hh"
#include "controls.hh"
#include "game/object.hh"
#include "lvector.hh"
#include "model.hh"

namespace hiemalia {
inline constexpr size_t maxShards = 128;

/*struct ExplosionShard {
    Point3D p0;
    Point3D p1;
    Point3D pos;
    Orient3D rot;
    Point3D dpos;
    Orient3D drot;
};*/

class Explosion : public GameObject {
  public:
    Explosion(const Point3D& pos, const GameObject& o, coord_t xm, coord_t ym,
              coord_t zm, float explspeed);
    Explosion(const Point3D& pos, const GameObject& o, const Model& m,
              coord_t xm, coord_t ym, coord_t zm, float explspeed);
    bool update(GameWorld& w, float delta);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    void adjustSpeed(coord_t s);
    void onMove(const Point3D& newPos);

  private:
    LimitedVector<Point3D, maxShards> shards_p0_;
    LimitedVector<Point3D, maxShards> shards_p1_;
    LimitedVector<Point3D, maxShards> shards_pos_;
    LimitedVector<Point3D, maxShards> shards_dpos_;
    LimitedVector<Orient3D, maxShards> shards_rot_;
    LimitedVector<Orient3D, maxShards> shards_drot_;

    Model tempModel_;
    float alpha_{1};
    float explspeed_{1};
    inline bool collideLineInternal(const Point3D& p1,
                                    const Point3D& p2) const {
        return false;
    }
    inline bool collideCuboidInternal(const Point3D& c1,
                                      const Point3D& c2) const {
        return false;
    }
    inline bool collideSphereInternal(const Point3D& p, coord_t r2) const {
        return false;
    }

    void cutShards(coord_t xm, coord_t ym, coord_t zm);
};
};  // namespace hiemalia

#endif  // M_GAME_EXPLODE_HH
