/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/world.cc: implementation of GameWorld

#include "game/world.hh"

#include "assets.hh"
#include "game/stage.hh"
#include "math.hh"

namespace hiemalia {

MoveRegion GameWorld::getPlayerMoveRegion() {
    const MoveRegion& s0 = getSectionById(stage->visible[0]).region;
    const MoveRegion& s1 = getSectionById(stage->visible[1]).region;
    return {lerp(s0.x0, progress_f, s1.x0), lerp(s0.x1, progress_f, s1.x1),
            lerp(s0.y0, progress_f, s1.y0), lerp(s0.y1, progress_f, s1.y1)};
}

void GameWorld::startNewStage() {
    player = std::make_unique<PlayerObject>();
    stage = std::make_unique<GameStage>(std::move(GameStage::load(++stageNum)));
    objects.clear();
}

void GameWorld::drawStage(SplinterBuffer& sbuf, Renderer3D& r3d) {
    stage->drawStage(sbuf, r3d, progress_f);
}

void GameWorld::moveForward(coord_t dist) {
    progress += dist;
    progress_f += dist;
    player->move(0, 0, dist);
    while (progress_f >= stageSectionLength) {
        progress_f -= stageSectionLength;
        ++sections;
        player->move(0, 0, -stageSectionLength);
        for (auto& obj : objects) obj->move(0, 0, -stageSectionLength);
        stage->nextSection();
    }

    auto& obj = stage->spawns;
    unsigned u = sections + stageSpawnDistance * stageDivision;
    while (!obj.empty() && obj.front().shouldSpawn(u, progress_f)) {
        LOG_TRACE("spawn");
        objects.emplace_back(std::move(obj.front().obj))->onSpawn(*this);
        obj.pop_front();
    }
}

PlayerObject& GameWorld::getPlayer() { return *player; }

}  // namespace hiemalia
