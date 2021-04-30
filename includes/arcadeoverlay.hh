/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/arcadeoverlay.hh: header file for the arcade mode menu overlay

#ifndef M_ARCADEOVERLAY_HH
#define M_ARCADEOVERLAY_HH

#include <string>

#include "defs.hh"
#include "lmodule.hh"
#include "mholder.hh"
#include "rendtext.hh"
#include "state.hh"

namespace hiemalia {
class ArcadeOverlay : public LogicModule {
  public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    bool run(GameState& state, float interval);

    DELETE_COPY(ArcadeOverlay);
    DEFAULT_MOVE(ArcadeOverlay);
    ArcadeOverlay(std::shared_ptr<ModuleHolder> holder);
    virtual ~ArcadeOverlay() noexcept = default;

    void visible(bool visible);
    void redraw(int credits, bool freePlay);

  private:
    static inline const std::string name_ = "ArcadeOverlay";
    static inline const std::string role_ = "arcade overlay";

    RendererText font_;
    SplinterBuffer buf_;
    std::shared_ptr<ModuleHolder> holder_;
    bool visible_{false};
    bool haveCredits_{false};
    float blink_{0};
};
};  // namespace hiemalia

#endif  // M_ARCADEOVERLAY_HH
