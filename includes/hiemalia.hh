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
#include "ibase.hh"
#include "logic.hh"
#include "msg.hh"
#include "video.hh"

namespace hiemalia {

template <typename T, typename... Ts>
void sendMessage(Ts&&... args) {
    T msg(std::forward<Ts>(args)...);
    MessageHandler<T>::deliver(msg);
}

class Hiemalia {
   public:
    Hiemalia(const std::string& command);
    ~Hiemalia();
    Hiemalia(const Hiemalia& copy) = delete;
    Hiemalia& operator=(const Hiemalia& copy) = delete;
    Hiemalia(Hiemalia&& move);
    Hiemalia& operator=(Hiemalia&& move);

    void args(std::vector<std::string> args);
    void run();

   private:
    std::string command_;
    std::string configFileName{"hiemalia.cfg"};
    GameState state_;
    MenuControlState controlsmenu_;
    std::shared_ptr<HostModule> host_;
    std::shared_ptr<InputModule> input_;
    std::unique_ptr<VideoEngine> video_;
    std::unique_ptr<AudioEngine> audio_;
    std::unique_ptr<LogicEngine> logic_;
};

};  // namespace hiemalia

#endif  // M_HIEMALIA_HH
