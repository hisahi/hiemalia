/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/gamemsg.hh: header file for game messages

#ifndef M_GAME_GAMEMSG_HH
#define M_GAME_GAMEMSG_HH

#include <variant>

namespace hiemalia {
enum class GameMessageType {
    AddScore,
    UpdateCamera,
    UpdateStatus,
    PauseGame,
    ContinueGame,
    ExitGame
};

struct GameMessage {
    GameMessageType type;

    inline static GameMessage addScore(unsigned score) {
        return GameMessage(GameMessageType::AddScore, score);
    }

    inline static GameMessage updateCamera() {
        return GameMessage(GameMessageType::UpdateCamera);
    }

    inline static GameMessage updateStatus() {
        return GameMessage(GameMessageType::UpdateStatus);
    }

    inline static GameMessage pauseGame() {
        return GameMessage(GameMessageType::PauseGame);
    }

    inline static GameMessage continueGame() {
        return GameMessage(GameMessageType::ContinueGame);
    }

    inline static GameMessage exitGame() {
        return GameMessage(GameMessageType::ExitGame);
    }

    inline unsigned getScore() const { return std::get<unsigned>(x); }

   private:
    GameMessage(GameMessageType t) : type(t) {}
    GameMessage(GameMessageType t, unsigned v) : type(t), x(v) {}
    std::variant<unsigned> x;
};
};  // namespace hiemalia

#endif  // M_GAME_GAMEMSG_HH
