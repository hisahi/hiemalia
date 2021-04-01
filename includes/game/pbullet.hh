/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/pbullet.hh: header file for bullet objects (game/pbullet.cc)

#ifndef M_GAME_PBULLET_HH
#define M_GAME_PBULLET_HH

#include "game/bullet.hh"
#include "model.hh"

namespace hiemalia {
class PlayerBullet : public BulletObject {
   public:
    PlayerBullet(const ModelPoint& v);
    bool doBulletTick(GameWorld& w, float delta);
    void impact(GameWorld& w, bool enemy);
    bool firedByPlayer() const;
    float getDamage() const;
    virtual ~PlayerBullet() {}
};
};  // namespace hiemalia

#endif  // M_GAME_PBULLET_HH
