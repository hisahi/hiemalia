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

#include "game/object.hh"
#include "game/player.hh"
#include "game/stage.hh"

namespace hiemalia {
class GameMain;

struct GameWorld {
    MoveRegion getPlayerMoveRegion();
    void startNewStage();
    void drawStage(SplinterBuffer& sbuf, Renderer3D& r3d);
    void moveForward(coord_t dist);
    PlayerObject& getPlayer();

   private:
    std::unique_ptr<PlayerObject> player;
    std::unique_ptr<GameStage> stage;
    std::vector<std::shared_ptr<GameObject>> objects;
    unsigned sections{0};
    coord_t progress{0};
    coord_t progress_f{0};
    unsigned int score{0};
    unsigned int highScore{0};
    int stageNum{0};
    int cycles{1};
    bool nextStage{true};
    int lives{3};
    friend class GameMain;
    friend class PlayerObject;
};
};  // namespace hiemalia

#endif  // M_GAME_WORLD_HH
