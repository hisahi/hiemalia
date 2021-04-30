/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// hiemalia.hh: includes for hiemalia.cc

#ifndef M_HIEMALIA_HH
#define M_HIEMALIA_HH

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "arcadeoverlay.hh"
#include "audio.hh"
#include "defs.hh"
#include "hbase.hh"
#include "input.hh"
#include "logic.hh"
#include "menu.hh"
#include "msg.hh"
#include "scores.hh"
#include "video.hh"

namespace hiemalia {

template <typename T>
void sendMessage(T msg) {
    MessageHandler<T>::deliver(msg);
}

template <typename T, typename... Ts>
void sendMessageMake(Ts&&... args) {
    MessageHandler<T>::deliver(T(std::forward<Ts>(args)...));
}

enum class HostMessageType {
    MainMenu,
    MainMenuFromDemo,
    GameStarted,
    DemoStarted,
    Quit,
    GotHighScore,
    EnteredHighScore
};

struct HostMessage {
    HostMessageType type;

    static HostMessage mainMenu() {
        return HostMessage(HostMessageType::MainMenu);
    }
    static HostMessage mainMenuFromDemo() {
        return HostMessage(HostMessageType::MainMenuFromDemo);
    }
    static HostMessage gameStarted() {
        return HostMessage(HostMessageType::GameStarted);
    }
    static HostMessage demoStarted() {
        return HostMessage(HostMessageType::DemoStarted);
    }
    static HostMessage gotHighScore(PartialHighScoreEntry&& e) {
        return HostMessage(HostMessageType::GotHighScore, std::move(e));
    }
    static HostMessage enteredHighScore() {
        return HostMessage(HostMessageType::EnteredHighScore);
    }
    static HostMessage quit() { return HostMessage(HostMessageType::Quit); }

    inline const PartialHighScoreEntry& highScoreEntry() const {
        dynamic_assert(type == HostMessageType::GotHighScore,
                       "wrong message type");
        return std::get<PartialHighScoreEntry>(x);
    }

  private:
    HostMessage(HostMessageType t) : type(t) {}
    HostMessage(HostMessageType t, PartialHighScoreEntry&& e)
        : type(t), x(std::move(e)) {}

    std::variant<PartialHighScoreEntry> x;
};

class Hiemalia : MessageHandler<HostMessage> {
  public:
    Hiemalia(const std::string& command);
    ~Hiemalia() = default;
    Hiemalia(const Hiemalia& copy) = delete;
    Hiemalia& operator=(const Hiemalia& copy) = delete;
    Hiemalia(Hiemalia&& move) noexcept;
    Hiemalia& operator=(Hiemalia&& move) noexcept;

    void gotMessage(const HostMessage& msg);
    void args(std::vector<std::string> args);
    void run();
    int addCredits(int credits);

  private:
    std::string command_;
    std::string configFileName{"hiemalia.cfg"};
    GameState state_;
    std::shared_ptr<HostModule> host_;
    std::shared_ptr<ModuleHolder> modules_;
    std::shared_ptr<ArcadeOverlay> overlay_;
    int credits_{0};

    void resetArcade();
};

extern "C" int hiemalia_tryAddCredits(int);

};  // namespace hiemalia

#endif  // M_HIEMALIA_HH
