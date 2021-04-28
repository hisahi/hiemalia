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
#include "gconfig.hh"
#include "lvector.hh"

namespace hiemalia {
class GameMain;
class EnemyObject;

constexpr int objectsMax = 512;
constexpr int stageCount = 4;
constexpr int defaultLives = 3;
inline const coord_t farObjectBackPlane =
    static_cast<coord_t>(1 * stageSpawnDistance);

template <typename T>
using ObjectPtrBase = std::shared_ptr<T>;
template <typename T>
using ObjectListBase = LimitedVector<ObjectPtrBase<T>, objectsMax>;
using ObjectPtr = ObjectPtrBase<GameObject>;
using ObjectList = ObjectListBase<GameObject>;
using BulletList = ObjectListBase<BulletObject>;
using EnemyList = ObjectListBase<EnemyObject>;

class GameWorld {
  public:
    GameWorld(ConfigSectionPtr<GameConfig> config);

    DELETE_COPY(GameWorld);
    DEFAULT_MOVE(GameWorld);
    inline virtual ~GameWorld(){};

    void startNewStage();
    void resetStage(coord_t t);
    void addScore(unsigned int p);
    void drawStage(SplinterBuffer& sbuf, Renderer3D& r3d);
    void go(float interval);
    void moveForward(coord_t dist);
    MoveRegion getPlayerMoveRegion() const;
    MoveRegion getPlayerMoveRegion0() const;
    MoveRegion getMoveRegionForZ(coord_t z) const;
    PlayerObject& getPlayer();
    bool isPlayerAlive() const;
    bool runPlayer(float interval);
    void renderPlayer(SplinterBuffer& sbuf, Renderer3D& r3d,
                      const Orient3D& envRot);
    coord_t getMoveSpeed() const;
    coord_t getMoveSpeedDelta() const;
    coord_t getObjectBackPlane() const;
    Orient3D getSectionRotation() const;
    Point3D rotateInSection(Point3D v, coord_t z) const;
    std::unique_ptr<PlayerObject>&& explodePlayer(
        std::unique_ptr<Explosion>&& expl);
    void explodeEnemy(GameObject& enemy, const Model& model);
    void explodeBoss(GameObject& enemy, const Model& model);
    void explodeBullet(BulletObject& bullet);
    const Point3D& getPlayerPosition();
    bool respawn();
    const EnemyList& getEnemies() const;
    const BulletList& getPlayerBullets() const;
    const BulletList& getEnemyBullets() const;
    void setNewSpeed(coord_t s, coord_t d);
    void setCheckpoint(coord_t z);
    void endStage();
    void endGame();
    void updateMoveSpeedInput(ControlState& controls, float delta);
    const GameDifficulty& difficulty() const noexcept;
    coord_t pushBoss(std::initializer_list<section_t> bossLoop, coord_t v);
    void popBoss(coord_t x);
    void onEnemyKilled(const GameObject& obj);
    bool shouldGetNABonus() const;
    int getBossesAlive() const;
    int continuesRemaining() const;
    void spendContinue();
    void addCredits(int n);

    template <typename T, typename... Ts>
    void spawn(const Point3D& p, Ts&&... args) {
        auto& o = objects.emplace_back(
            std::make_shared<T>(p, std::forward<Ts>(args)...));
        o->onSpawn(*this);
    }
    template <typename T, typename... Ts>
    void spawnEnemy(const Point3D& p, Ts&&... args) {
        auto& o = enemies.emplace_back(
            std::make_shared<T>(p, std::forward<Ts>(args)...));
        o->onSpawn(*this);
    }
    template <typename T, typename... Ts>
    void firePlayerBullet(const Point3D& p, Ts&&... args) {
        auto& b = playerBullets.emplace_back(
            std::make_shared<T>(p, std::forward<Ts>(args)...));
        b->onSpawn(*this);
    }
    template <typename T, typename... Ts>
    void fireEnemyBullet(const Point3D& p, const Point3D& v, Ts&&... args) {
        auto& b = enemyBullets.emplace_back(
            std::make_shared<T>(p, v, std::forward<Ts>(args)...));
        b->onSpawn(*this);
    }

  private:
    ConfigSectionPtr<GameConfig> config_;
    std::unique_ptr<PlayerObject> player;
    std::unique_ptr<Explosion> playerExplosion;
    std::unique_ptr<GameStage> stage;
    ObjectList objects;
    EnemyList enemies;
    BulletList playerBullets;
    BulletList enemyBullets;
    unsigned sections{0};
    coord_t checkpoint{0};
    coord_t progress_f{0};
    coord_t moveSpeedBase{0};
    coord_t moveSpeedDst{1.25};
    coord_t moveSpeedCtl{0};
    coord_t moveSpeedVel{2};
    coord_t moveSpeedFac{1};
    coord_t moveSpeedOverride{0};
    int bossLevel{0};
    coord_t bossSlideTime{0};
    unsigned long score{0};
    unsigned long highScore{0};
    int stageNum{0};
    int cycle{1};
    bool nextStage{true};
    int lives{defaultLives};
    int continues_{0};
    int continuesUsed_{0};
    int bosses_{0};
    int killed_{0};
    Point3D lastPos{0, 0, 0};
    GameDifficulty difficulty_;

    void moveForwardSkip(coord_t dist);

    friend class GameMain;
    friend class PlayerObject;
};
};  // namespace hiemalia

#endif  // M_GAME_WORLD_HH
