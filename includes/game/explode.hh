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
#include "model.hh"

namespace hiemalia {
struct ExplosionShard {
    ModelPoint p0;
    ModelPoint p1;
    ModelPoint pos;
    Rotation3D rot;
    ModelPoint dpos;
    Rotation3D drot;
};

class Explosion : public GameObject {
   public:
    Explosion(const GameObject& o, coord_t xm, coord_t ym, coord_t zm);
    bool update(GameWorld& w, float delta);
    void renderSpecial(SplinterBuffer& sbuf, Renderer3D& r3d);
    void adjustSpeed(coord_t s);

   private:
    std::vector<ExplosionShard> shards;
    Model tempModel;
    float alpha{1};
    inline bool collideLineInternal(const ModelPoint& p1,
                                    const ModelPoint& p2) const {
        return false;
    }
    inline bool collideCuboidInternal(const ModelPoint& c1,
                                      const ModelPoint& c2) const {
        return false;
    }
    inline bool collideSphereInternal(const ModelPoint& p, coord_t r2) const {
        return false;
    }
};
};  // namespace hiemalia

#endif  // M_GAME_EXPLODE_HH
