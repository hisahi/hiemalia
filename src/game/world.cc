/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/world.cc: implementation of GameWorld

#include "game/world.hh"

#include "assets.hh"
#include "game/gamemsg.hh"
#include "game/stage.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "sounds.hh"

namespace hiemalia {

MoveRegion GameWorld::getPlayerMoveRegion() {
    const MoveRegion& s0 = getSectionById(stage->visible[0]).region;
    const MoveRegion& s1 = getSectionById(stage->visible[1]).region;
    return {lerp(s0.x0, progress_f, s1.x0), lerp(s0.x1, progress_f, s1.x1),
            lerp(s0.y0, progress_f, s1.y0), lerp(s0.y1, progress_f, s1.y1)};
}

void GameWorld::startNewStage() {
    ++stageNum;
    checkpoint = 0;
    resetStage(0);
}

void GameWorld::resetStage(coord_t t) {
    player = std::make_unique<PlayerObject>();
    stage = std::make_unique<GameStage>(std::move(GameStage::load(stageNum)));
    progress = 0;
    progress_f = 0;
    sections = 0;
    objects.clear();
    if (t > 0) {
        moveForward(t);
        objects.clear();
    }
}

void GameWorld::addScore(unsigned int p) {
    if (highScore < (score += p)) {
        highScore = score;
    }
    sendMessage(GameMessage::updateStatus());
}

void GameWorld::drawStage(SplinterBuffer& sbuf, Renderer3D& r3d) {
    stage->drawStage(sbuf, r3d, progress_f);
}

void GameWorld::moveForward(coord_t dist) {
    if (!player) return;
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
        objects.emplace_back(std::move(obj.front().obj))->onSpawn(*this);
        obj.pop_front();
    }
}

bool GameWorld::isPlayerAlive() const { return player != nullptr; }

PlayerObject& GameWorld::getPlayer() { return *player; }

bool GameWorld::runPlayer(float interval) {
    if (player)
        return player->update(*this, interval);
    else if (playerExplosion) {
        bool b = playerExplosion->update(*this, interval);
        if (!b) playerExplosion = nullptr;
        return b;
    } else
        return false;
}

void GameWorld::renderPlayer(SplinterBuffer& sbuf, Renderer3D& r3d) {
    if (player)
        player->render(sbuf, r3d);
    else if (playerExplosion)
        playerExplosion->render(sbuf, r3d);
}

coord_t GameWorld::getMoveSpeed() const { return moveSpeed; }

std::unique_ptr<PlayerObject>&& GameWorld::explodePlayer(
    std::unique_ptr<Explosion>&& expl) {
    playerExplosion = std::move(expl);
    playerExplosion->adjustSpeed(moveSpeed);
    dynamic_assert(playerExplosion != nullptr, "null explosion");
    sendMessage(AudioMessage::playSound(SoundEffect::PlayerExplode));
    return std::move(player);
}

const ModelPoint& GameWorld::getPlayerPosition() {
    if (player)
        return lastPos = player->pos;
    else if (playerExplosion)
        return lastPos = playerExplosion->pos;
    else
        return lastPos;
}

bool GameWorld::respawn() {
    if (--lives < 0) return false;
    playerExplosion = nullptr;
    resetStage(checkpoint);
    sendMessage(GameMessage::updateStatus());
    return true;
}

}  // namespace hiemalia
