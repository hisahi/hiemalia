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
#include <vector>

#include "audio.hh"
#include "defs.hh"
#include "hbase.hh"
#include "input.hh"
#include "logic.hh"
#include "menu.hh"
#include "msg.hh"
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

enum class HostMessageType { MainMenu, Quit };

struct HostMessage {
    HostMessageType type;

    static HostMessage mainMenu() {
        return HostMessage(HostMessageType::MainMenu);
    }
    static HostMessage quit() { return HostMessage(HostMessageType::Quit); }

   private:
    HostMessage(HostMessageType t) : type(t) {}
};

class Hiemalia : MessageHandler<HostMessage> {
   public:
    Hiemalia(const std::string& command);
    ~Hiemalia();
    Hiemalia(const Hiemalia& copy) = delete;
    Hiemalia& operator=(const Hiemalia& copy) = delete;
    Hiemalia(Hiemalia&& move);
    Hiemalia& operator=(Hiemalia&& move);

    void gotMessage(const HostMessage& msg);
    void args(std::vector<std::string> args);
    void run();

   private:
    std::string command_;
    std::string configFileName{"hiemalia.cfg"};
    GameState state_;
    std::shared_ptr<HostModule> host_;
    std::shared_ptr<ModuleHolder> modules_;
};

};  // namespace hiemalia

#endif  // M_HIEMALIA_HH
