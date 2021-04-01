/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/player.hh: header file for the player object (game/player.cc)

#ifndef M_GAME_PLAYER_HH
#define M_GAME_PLAYER_HH

#include "controls.hh"
#include "game/explode.hh"
#include "game/object.hh"
#include "game/stage.hh"
#include "model.hh"

namespace hiemalia {
class PlayerObject : public GameObject, public ObjectDamageable {
   public:
    PlayerObject();
    void updateInput(ControlState& controls);
    bool update(GameWorld& w, float delta);
    bool shouldCatchCheckpoints() const;
    void wallContact(coord_t x, coord_t y, coord_t z);

   private:
    ControlState inputs_;
    ModelPoint vel{0, 0, 0};
    std::unique_ptr<Explosion> explodeObject_;
    float fireInterval_{0};
    bool woundedBird_{false};
    coord_t wbird_mul_{0};
    coord_t wbird_fr_{0};
    Rotation3D wbird_vel_{0, 0, 0};

    void onDamage(GameWorld& w, float dmg, const ModelPoint& pointOfContact);
    void onDeath(GameWorld& w);
    bool collideLineInternal(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboidInternal(const ModelPoint& c1,
                               const ModelPoint& c2) const;
    bool collideSphereInternal(const ModelPoint& p, coord_t r2) const;
    void doWoundedBird(float delta);
    void inputsVelocity(float delta);
    void inputsAngles(float delta);
    void checkBounds(const MoveRegion& r, GameWorld& w);
    void doFire(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_PLAYER_HH
