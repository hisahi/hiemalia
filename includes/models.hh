/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// models.hh: header file for model file name list

#ifndef M_MODELS_HH
#define M_MODELS_HH

#include <array>
#include <string>

#include "array.hh"

namespace hiemalia {
enum class GameModel {
    PlayerShip,
    BoxModel,
    TitleCubeModel,
    BulletPlayer,
    EnemyShard,
    EnemyGunboat,
    BulletEnemy,
    BulletEnemy2,
    EnemyVolcano,
    EnemyChevron,
    EnemyFighter,
    EnemyWave,
    EnemyTurret,
    EnemyTurretStand,
    EnemyTurretCannon,
    SlidingBoxModel,
    CrackedWindow,
    EnemyBoss0,
    EnemyBoss1,
    Donut,
    CavePillar0,
    CavePillar1,
    CavePillar2,
    Column0,
    Column1,
    Column2,
    Column3,
    EnemyGunboat2,
    EnemyWalker,
    EnemyWalkerLeg,
    EnemySpider,
    EnemySpiderLeg,

    EndOfModels
};

inline auto modelFileNames = hiemalia::makeArray<std::string>({
    "ship.3d",    "box.3d",     "tcube.3d",    "tetra.3d",   "shard.3d",
    "gunboat.3d", "ebullet.3d", "ebullet2.3d", "volcano.3d", "chevron.3d",
    "fighter.3d", "wave.3d",    "turret.3d",   "turretp.3d", "turretc.3d",
    "sbox.3d",    "shipwcr.3d", "boss0.3d",    "boss1.3d",   "donut.3d",
    "cavep0.3d",  "cavep1.3d",  "cavep2.3d",   "column0.3d", "column1.3d",
    "column2.3d", "column3.3d", "gunboat2.3d", "walker.3d",  "walkerl.3d",
    "spider.3d",  "spiderl.3d",
});
};  // namespace hiemalia

#endif  // M_MODELS_HH
