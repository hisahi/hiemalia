/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/ebullet.hh: header file for simple enemy bullets (game/ebullet.cc)

#ifndef M_GAME_EBULLET_HH
#define M_GAME_EBULLET_HH

#include "game/bullet.hh"
#include "model.hh"

namespace hiemalia {
class EnemyBulletSimple : public BulletObject {
   public:
    EnemyBulletSimple(const ModelPoint& v);
    bool doBulletTick(GameWorld& w, float delta);
    void impact(GameWorld& w, bool enemy);
    bool firedByPlayer() const;
    float getDamage() const;
    virtual ~EnemyBulletSimple() {}
};
};  // namespace hiemalia

#endif  // M_GAME_EBULLET_HH
