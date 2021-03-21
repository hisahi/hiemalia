/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// test.hh: header file for test modes (test.cc)

#ifndef M_TEST_HH
#define M_TEST_HH

#include <string>

#include "defs.hh"
#include "inherit.hh"
#include "lmodule.hh"
#include "menu.hh"
#include "model.hh"
#include "msg.hh"
#include "rend3d.hh"
#include "rendtext.hh"
#include "state.hh"

namespace hiemalia {
class TestMode : public LogicModule, MessageHandler<MenuMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);

    DELETE_COPY(TestMode);
    TestMode(TestMode&& move) noexcept;
    TestMode& operator=(TestMode&& move) noexcept;
    TestMode();
    virtual ~TestMode() noexcept {}

   private:
    static inline const std::string name_ = "TestMode";
    static inline const std::string role_ = "test module";
    void setMode(int mode);
    Model cube_;
    ModelPoint pos_;
    Rotation3D rot_;
    coord_t scale_;
    Renderer3D rend_;
    RendererText font_;
    SplinterBuffer text_;
    int mode_;
};
};  // namespace hiemalia

#endif  // M_TEST_HH
