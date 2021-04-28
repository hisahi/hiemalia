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
    UpdateStatus,
    CreditAdded,
    PauseGame,
    ResumeGame,
    ContinueGame,
    StageComplete,
    GameComplete,
    ExitGame,
    ShakeCamera,
    AddCredits
};

struct GameMessage {
    GameMessageType type;

    inline static GameMessage updateStatus() {
        return GameMessage(GameMessageType::UpdateStatus);
    }

    inline static GameMessage creditAdded() {
        return GameMessage(GameMessageType::CreditAdded);
    }

    inline static GameMessage pauseGame() {
        return GameMessage(GameMessageType::PauseGame);
    }

    inline static GameMessage resumeGame() {
        return GameMessage(GameMessageType::ResumeGame);
    }

    inline static GameMessage continueGame() {
        return GameMessage(GameMessageType::ContinueGame);
    }

    inline static GameMessage stageComplete() {
        return GameMessage(GameMessageType::StageComplete);
    }

    inline static GameMessage gameComplete() {
        return GameMessage(GameMessageType::GameComplete);
    }

    inline static GameMessage exitGame() {
        return GameMessage(GameMessageType::ExitGame);
    }

    inline static GameMessage shakeCamera(coord_t factor) {
        return GameMessage(GameMessageType::ShakeCamera, factor);
    }

    inline static GameMessage addCredits(unsigned credits) {
        return GameMessage(GameMessageType::AddCredits, credits);
    }

    inline coord_t getFactor() const {
        dynamic_assert(type == GameMessageType::ShakeCamera, "wrong type");
        return std::get<coord_t>(x);
    }
    inline unsigned getCredits() const {
        dynamic_assert(type == GameMessageType::AddCredits, "wrong type");
        return std::get<unsigned>(x);
    }

  private:
    GameMessage(GameMessageType t) : type(t) {}
    GameMessage(GameMessageType t, unsigned v) : type(t), x(v) {}
    GameMessage(GameMessageType t, coord_t v) : type(t), x(v) {}
    std::variant<unsigned, coord_t> x;
};
};  // namespace hiemalia

#endif  // M_GAME_GAMEMSG_HH
