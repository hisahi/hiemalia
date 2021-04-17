/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/wasp.hh: header file for game/enemy/wasp.cc

#ifndef M_GAME_ENEMY_WASP_HH
#define M_GAME_ENEMY_WASP_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyWasp : public EnemyObject {
  public:
    EnemyWasp(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    const std::vector<ExtraCollision>& exCollisions() const;

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    coord_t anim_{0};
    float fireTime_{0};
    std::vector<ExtraCollision> exCol_;
    LoadedGameModel wingModel_;
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_WASP_HH
