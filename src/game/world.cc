/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/world.cc: implementation of GameWorld

#include "game/world.hh"

#include <cmath>

#include "assets.hh"
#include "game/enemy.hh"
#include "game/gamemsg.hh"
#include "game/stage.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "sounds.hh"

namespace hiemalia {

MoveRegion GameWorld::getMoveRegionForZ(coord_t z) const {
    coord_t fz = z * stageDivision - stageSectionOffset;
    auto u = floatToWholeFrac<int>(fz);
    if (u < 0) return getSectionById(stage->visible.front()).region;
    if (u + 1 >= static_cast<int>(stage->visible.size()))
        return getSectionById(stage->visible.back()).region;
    const MoveRegion& s0 = getSectionById(stage->visible[u]).region;
    const MoveRegion& s1 = getSectionById(stage->visible[u + 1]).region;
    return {lerp(s0.x0, fz, s1.x0), lerp(s0.x1, fz, s1.x1),
            lerp(s0.y0, fz, s1.y0), lerp(s0.y1, fz, s1.y1)};
}

MoveRegion GameWorld::getPlayerMoveRegion() const {
    return getMoveRegionForZ(player->pos.z);
}

Rotation3D GameWorld::getSectionRotation() const {
    int u =
        static_cast<int>(player->pos.z * stageDivision - stageSectionOffset);
    return getSectionById(stage->visible[u]).rotation *
           (player->pos.z * stageDivision);
}

ModelPoint GameWorld::rotateInSection(ModelPoint v, coord_t z) const {
    int u = static_cast<int>(z * stageDivision - stageSectionOffset);
    return Matrix3D::rotate(getSectionById(stage->visible[u]).rotation)
        .project(v);
}

void GameWorld::startNewStage() {
    ++stageNum;
    if (stageNum > stageCount) {
        ++cycle;
        stageNum = 1;
        moveSpeedFac *= 1.1;
    }
    checkpoint = 0;
    resetStage(0);
}

void GameWorld::resetStage(coord_t t) {
    player = std::make_unique<PlayerObject>();
    stage = std::make_unique<GameStage>(GameStage::load(stageNum));
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
    score += p;
    if (highScore < score) {
        highScore = score;
    }
    sendMessage(GameMessage::updateStatus());
}

coord_t GameWorld::getObjectBackPlane() const {
    return stage->getObjectBackPlane(0);
}

void GameWorld::drawStage(SplinterBuffer& sbuf, Renderer3D& r3d) {
    stage->drawStage(sbuf, r3d, 0);
}

void GameWorld::moveForward(coord_t dist) {
    if (!player) return;
    progress += dist;
    progress_f += dist;
    player->move(0, 0, dist);
    while (progress_f >= stageSectionLength) {
        const coord_t moveDist = -stageSectionLength;
        progress_f -= stageSectionLength;
        ++sections;
        player->move(0, 0, moveDist);
        for (auto& obj : objects) obj->move(0, 0, moveDist);
        for (auto& obj : playerBullets) obj->move(0, 0, moveDist);
        for (auto& obj : enemyBullets) obj->move(0, 0, moveDist);
        stage->nextSection();
    }

    auto& obj = stage->spawns;
    unsigned u = sections + stageSpawnDistance * stageDivision;
    while (!obj.empty() && obj.front().shouldSpawn(u, progress_f)) {
        objects.emplace_back(std::move(obj.front().obj))->onSpawn(*this);
        obj.pop_front();
    }
}

void GameWorld::go(float interval) {
    moveForward(interval * getMoveSpeed());
    if (moveSpeedBase < moveSpeedDst) {
        moveSpeedBase =
            std::min(moveSpeedDst, moveSpeedBase + interval * moveSpeedVel);
    } else if (moveSpeedBase > moveSpeedDst) {
        moveSpeedBase =
            std::max(moveSpeedDst, moveSpeedBase - interval * moveSpeedVel);
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

void GameWorld::renderPlayer(SplinterBuffer& sbuf, Renderer3D& r3d,
                             const Rotation3D& envRot) {
    if (player) {
        if (!envRot.isZero()) {
            Rotation3D rotBackup = player->rot;
            player->rot += envRot;
            player->render(sbuf, r3d);
            player->rot = rotBackup;
        } else
            player->render(sbuf, r3d);
    } else if (playerExplosion)
        playerExplosion->render(sbuf, r3d);
}

coord_t GameWorld::getMoveSpeed() const {
    return moveSpeedBase * moveSpeedFac * pow<coord_t>(2, moveSpeedCtl);
}

void GameWorld::updateMoveSpeedInput(ControlState& inputs, float delta) {
    if (inputs.back) {
        moveSpeedCtl = std::max<coord_t>(-1, moveSpeedCtl - delta * 2);
    } else if (inputs.forward) {
        moveSpeedCtl = std::min<coord_t>(1, moveSpeedCtl + delta * 4);
    } else if (moveSpeedCtl > 0) {
        moveSpeedCtl = std::max<coord_t>(0, moveSpeedCtl - delta * 2);
    } else if (moveSpeedCtl < 0) {
        moveSpeedCtl = std::min<coord_t>(0, moveSpeedCtl + delta * 4);
    }
}

std::unique_ptr<PlayerObject>&& GameWorld::explodePlayer(
    std::unique_ptr<Explosion>&& expl) {
    playerExplosion = std::move(expl);
    playerExplosion->adjustSpeed(getMoveSpeed() * 4);
    dynamic_assert(playerExplosion != nullptr, "null explosion");
    sendMessage(AudioMessage::stopMusic());
    sendMessage(AudioMessage::playSound(SoundEffect::PlayerExplode));
    return std::move(player);
}

void GameWorld::explodeEnemy(GameObject& obj, const Model& model) {
    auto expl = std::make_shared<Explosion>(obj, 0.0, 0.0, 0.0, 2.0f);
    expl->adjustSpeed(4.0);
    objects.push_back(std::move(expl));
}

void GameWorld::explodeBullet(BulletObject& b) {
    objects.push_back(std::make_shared<Explosion>(b, 0.0, 0.0, 0.0, 8.0f));
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

void GameWorld::setCheckpoint() {
    if (!isPlayerAlive() || !player->playerInControl()) return;
    checkpoint = player->pos.z;
}

void GameWorld::setNewSpeed(coord_t s, coord_t d) {
    moveSpeedDst = s;
    moveSpeedVel = d;
}

void GameWorld::endStage() {
    if (!isPlayerAlive() || !player->playerInControl()) return;
    sendMessage(GameMessage::stageComplete());
}

const BulletList& GameWorld::getPlayerBullets() const { return playerBullets; }

const BulletList& GameWorld::getEnemyBullets() const { return enemyBullets; }

}  // namespace hiemalia
