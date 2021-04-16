/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/spider.hh: header file for game/enemy/spider.cc

#ifndef M_GAME_ENEMY_SPIDER_HH
#define M_GAME_ENEMY_SPIDER_HH

#include <array>

#include "game/enemy.hh"

namespace hiemalia {
class EnemySpider : public EnemyObject {
  public:
    EnemySpider(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    LoadedGameModel leg_;
    coord_t walkMul_{0};
    coord_t walkCycle_{0};
    coord_t height_{0};
    bool pounce_{false};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_SPIDER_HH
