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
    PlayerObject(const Point3D& pos);
    void updateInput(const ControlState& controls);
    bool update(GameWorld& w, float delta);
    void updateGameEnd(GameWorld& w, float delta);
    bool playerInControl() const;
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    void enemyContact();
    void wallContact(coord_t x, coord_t y, coord_t z);

  private:
    ControlState inputs_;
    std::unique_ptr<Explosion> explodeObject_;
    float fireInterval_{0};
    bool woundedBird_{false};
    coord_t wbird_mul_{0};
    coord_t wbird_fr_{0};
    Orient3D wbird_vel_{0, 0, 0};
    coord_t shipRadius_;
    coord_t dropY_{0};

    void onDamage(GameWorld& w, float dmg, const Point3D& pointOfContact);
    void onDeath(GameWorld& w);
    void doWoundedBird(float delta);
    void inputsVelocity(float delta);
    void inputsAngles(float delta);
    void checkBounds(const MoveRegion& r, const MoveRegion& r0, GameWorld& w);
    void doFire(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_PLAYER_HH
