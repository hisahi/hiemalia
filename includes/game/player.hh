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
#include "game/object.hh"
#include "model.hh"

namespace hiemalia {
class PlayerObject : public GameObject, ObjectDamageable {
   public:
    PlayerObject();
    void updateInput(ControlState& controls);
    bool update(GameWorld& w, float delta);

   private:
    ControlState inputs;
    ModelPoint vel{0, 0, 0};
    void onDamage(float dmg);
    void onDeath();
    bool collideLineInternal(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboidInternal(const ModelPoint& c1,
                               const ModelPoint& c2) const;
    bool collideSphereInternal(const ModelPoint& p, coord_t r2) const;

    void inputsVelocity(float delta);
    void inputsAngles(float delta);
    void checkBounds(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_PLAYER_HH
