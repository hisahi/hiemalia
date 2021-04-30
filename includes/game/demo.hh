/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/demo.hh: header file for the demo mode

#ifndef M_GAME_DEMO_HH
#define M_GAME_DEMO_HH

#include <deque>
#include <string>

#include "buttons.hh"
#include "input.hh"

namespace hiemalia {
enum class DemoCommandType { DemoEnd, ButtonDown, ButtonUp };

struct DemoCommand {
    double time;
    DemoCommandType type;
    ControlInput input;
};

class DemoFile {
  public:
    static DemoFile loadDemo(const std::string& file);
    bool runDemo(float dt);
    inline const ControlState& getInputs() const noexcept { return input_; }
    inline int stage() const noexcept { return stageNum_; }
    inline coord_t offset() const noexcept { return checkpoint_; }
    void reset();

  private:
    int stageNum_{1};
    double t_{0};
    coord_t checkpoint_{0};
    size_t commandIndex_{0};
    std::vector<DemoCommand> commands_;
    ControlState input_;
};

std::shared_ptr<DemoFile> getNextDemo();
};  // namespace hiemalia

#endif  // M_GAME_DEMO_HH
