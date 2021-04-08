/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/turret.hh: header file for game/enemy/turret.cc

#ifndef M_GAME_ENEMY_TURRET_HH
#define M_GAME_ENEMY_TURRET_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyTurret : public EnemyObject {
  public:
    EnemyTurret(const Point3D& pos, Orient3D rot);
    bool doEnemyTick(GameWorld& w, float delta);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);
    bool hitsInternal(const GameObject& obj) const;

  private:
    void aim(GameWorld& w, float delta);
    float fireTime_{0};
    LoadedGameModel baseModel_;
    Orient3D baseRot_;
    Orient3D targetRot_;
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_TURRET_HH
