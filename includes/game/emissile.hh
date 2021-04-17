/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/emissile.hh: header file for enemy missiles (game/emissile.cc)

#ifndef M_GAME_EMISSILE_HH
#define M_GAME_EMISSILE_HH

#include "game/bullet.hh"
#include "model.hh"

namespace hiemalia {
class EnemyMissileHoming : public BulletObject {
  public:
    EnemyMissileHoming(const Point3D& pos, const Point3D& v);
    virtual bool doBulletTick(GameWorld& w, float delta);
    void impact(GameWorld& w, bool enemy);
    bool firedByPlayer() const;
    float getDamage() const;
    virtual ~EnemyMissileHoming() {}

  private:
    coord_t roll_{0};
};

};  // namespace hiemalia

#endif  // M_GAME_EMISSILE_HH
