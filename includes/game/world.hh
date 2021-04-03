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
#include "game/diffic.hh"
#include "game/explode.hh"
#include "game/object.hh"
#include "game/player.hh"
#include "game/stage.hh"
#include "lvector.hh"

namespace hiemalia {
class GameMain;

constexpr int objectsMax = 128;
constexpr int stageCount = 4;
inline const coord_t farObjectBackPlane =
    static_cast<coord_t>(2 * stageSpawnDistance);

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
    void go(float interval);
    void moveForward(coord_t dist);
    MoveRegion getPlayerMoveRegion() const;
    MoveRegion getMoveRegionForZ(coord_t z) const;
    PlayerObject& getPlayer();
    bool isPlayerAlive() const;
    bool runPlayer(float interval);
    void renderPlayer(SplinterBuffer& sbuf, Renderer3D& r3d,
                      const Rotation3D& envRot);
    coord_t getMoveSpeed() const;
    coord_t getObjectBackPlane() const;
    Rotation3D getSectionRotation() const;
    ModelPoint rotateInSection(ModelPoint v, coord_t z) const;
    std::unique_ptr<PlayerObject>&& explodePlayer(
        std::unique_ptr<Explosion>&& expl);
    void explodeEnemy(GameObject& enemy, const Model& model);
    void explodeBullet(BulletObject& bullet);
    const ModelPoint& getPlayerPosition();
    bool respawn();
    const BulletList& getPlayerBullets() const;
    const BulletList& getEnemyBullets() const;
    void setNewSpeed(coord_t s, coord_t d);
    void setCheckpoint();
    void endStage();
    void updateMoveSpeedInput(ControlState& controls, float delta);
    inline const GameDifficulty& difficulty() { return difficulty_; }

    template <typename T, typename... Ts>
    void firePlayerBullet(ModelPoint p, Ts&&... args) {
        auto& b = playerBullets.emplace_back(
            std::make_shared<T>(std::forward<Ts>(args)...));
        b->setPosition(p);
        b->onSpawn(*this);
    }
    template <typename T, typename... Ts>
    void fireEnemyBullet(ModelPoint p, ModelPoint v, Ts&&... args) {
        auto& b = enemyBullets.emplace_back(
            std::make_shared<T>(v, std::forward<Ts>(args)...));
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
    coord_t moveSpeedBase{0};
    coord_t moveSpeedDst{1.25};
    coord_t moveSpeedCtl{0};
    coord_t moveSpeedVel{2};
    coord_t moveSpeedFac{1};
    unsigned long score{0};
    unsigned long highScore{0};
    int stageNum{0};
    int cycle{1};
    bool nextStage{true};
    int lives{3};
    ModelPoint lastPos{0, 0, 0};
    GameDifficulty difficulty_{1};
    friend class GameMain;
    friend class PlayerObject;
};
};  // namespace hiemalia

#endif  // M_GAME_WORLD_HH
