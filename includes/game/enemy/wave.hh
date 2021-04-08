/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/wave.hh: header file for game/enemy/wave.cc

#ifndef M_GAME_ENEMY_WAVE_HH
#define M_GAME_ENEMY_WAVE_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyWave : public EnemyObject {
  public:
    EnemyWave(const Point3D& pos, coord_t sx, coord_t sy, coord_t sz, coord_t v,
              coord_t p);
    bool doEnemyTick(GameWorld& w, float delta);
    void onMove(const Point3D& newPos);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    Point3D off_;
    coord_t v_;
    coord_t x_;
    Point3D pos_;
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_WAVE_HH
