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
    EnemyVolcano,
    EnemyChevron,

    EndOfModels
};

inline auto modelFileNames = hiemalia::to_array<std::string>({
    "ship.3d", "box.3d", "tcube.3d", "tetra.3d", "shard.3d", "gunboat.3d",
    "ebullet.3d", "volcano.3d", "chevron.3d"
    // more
});
};  // namespace hiemalia

#endif  // M_MODELS_HH
