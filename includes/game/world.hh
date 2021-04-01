/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/world.hh: header file for the game world

#ifndef M_GAME_WORLD_HH
#define M_GAME_WORLD_HH

#include <memory>
#include <vector>

#include "game/bullet.hh"
#include "game/explode.hh"
#include "game/object.hh"
#include "game/player.hh"
#include "game/stage.hh"
#include "lvector.hh"

namespace hiemalia {
class GameMain;

constexpr int objectsMax = 128;
constexpr int stageCount = 4;

template <typename T>
using ObjectPtrBase = std::shared_ptr<T>;
template <typename T>
using ObjectListBase = LimitedVector<ObjectPtrBase<T>, objectsMax>;
using ObjectPtr = ObjectPtrBase<GameObject>;
using ObjectList = ObjectListBase<GameObject>;
using BulletList = ObjectListBase<BulletObject>;

struct GameWorld {
    void startNewStage();
    void resetStage(coord_t t);
    void addScore(unsigned int p);
    void drawStage(SplinterBuffer& sbuf, Renderer3D& r3d);
    void moveForward(coord_t dist);
    MoveRegion getPlayerMoveRegion() const;
    MoveRegion getMoveRegionForZ(coord_t z) const;
    PlayerObject& getPlayer();
    bool isPlayerAlive() const;
    bool runPlayer(float interval);
    void renderPlayer(SplinterBuffer& sbuf, Renderer3D& r3d);
    coord_t getMoveSpeed() const;
    std::unique_ptr<PlayerObject>&& explodePlayer(
        std::unique_ptr<Explosion>&& expl);
    void explodeBullet(BulletObject& bullet);
    const ModelPoint& getPlayerPosition();
    bool respawn();
    const BulletList& getPlayerBullets() const;
    const BulletList& getEnemyBullets() const;
    void setCheckpoint();
    void endStage();

    template <typename T, typename... Ts>
    void firePlayerBullet(ModelPoint p, Ts&&... args) {
        auto& b = playerBullets.emplace_back(
            std::make_shared<T>(std::forward<Ts>(args)...));
        b->setPosition(p);
        b->onSpawn(*this);
    }
    template <typename T, typename... Ts>
    void fireEnemyBullet(ModelPoint p, Ts&&... args) {
        auto& b = enemyBullets.emplace_back(
            std::make_shared<T>(std::forward<Ts>(args)...));
        b->setPosition(p);
        b->onSpawn(*this);
    }

   private:
    std::unique_ptr<PlayerObject> player;
    std::unique_ptr<Explosion> playerExplosion;
    std::unique_ptr<GameStage> stage;
    BulletList playerBullets;
    BulletList enemyBullets;
    ObjectList objects;
    unsigned sections{0};
    coord_t checkpoint{0};
    coord_t progress{0};
    coord_t progress_f{0};
    coord_t moveSpeed{0};
    unsigned int score{0};
    unsigned int highScore{0};
    int stageNum{0};
    int cycle{1};
    bool nextStage{true};
    int lives{3};
    ModelPoint lastPos{0, 0, 0};
    friend class GameMain;
    friend class PlayerObject;
};
};  // namespace hiemalia

#endif  // M_GAME_WORLD_HH
