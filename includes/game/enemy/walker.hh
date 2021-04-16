/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/walker.hh: header file for game/enemy/walker.cc

#ifndef M_GAME_ENEMY_WALKER_HH
#define M_GAME_ENEMY_WALKER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyWalker : public EnemyObject {
  public:
    EnemyWalker(const Point3D& pos, int pattern);
    bool doEnemyTick(GameWorld& w, float delta);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    void onSpawn(GameWorld& w);
    const std::vector<ExtraCollision>& exCollisions() const;

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    int pattern_{0};
    float fireTime_{0};
    coord_t pitchMul_{0};
    coord_t leftAngle_{0};
    coord_t rightAngle_{0};
    coord_t leftPitch_{0};
    coord_t rightPitch_{0};
    coord_t height_{0};
    LoadedGameModel leftLeg_;
    LoadedGameModel rightLeg_;
    std::vector<ExtraCollision> exCol_;

    void doWalk(GameWorld& w, float delta);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_WALKER_HH
