/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/nameentr.hh: header file for the name entry screen (game/nameentr.cc)

#ifndef M_GAME_NAMEENTR_HH
#define M_GAME_NAMEENTR_HH

#include <array>
#include <string>
#include <vector>

#include "defs.hh"
#include "lvector.hh"
#include "menu.hh"
#include "mholder.hh"
#include "rend2d.hh"
#include "sbuf.hh"
#include "scores.hh"

namespace hiemalia {

struct FireworkShard {
    Point2D pos;
    Point2D rot;
    Point2D vpos;
    Point2D vrot;
};

class Firework {
  public:
    Firework();
    bool update(SplinterBuffer& sbuf, Renderer2D& rend, float interval);

  private:
    Point2D pos;
    Point2D vel;
    bool explode_{false};
    Color explodeColor_;
    float explodeAlpha_{1.0f};
    float explodeFade_{1.0f};
    float explodeSize_{1.0f};
    std::vector<FireworkShard> explodePieces_;

    void explode();
    bool updateRocket(SplinterBuffer& sbuf, Renderer2D& rend, float interval);
    bool updateExplode(SplinterBuffer& sbuf, Renderer2D& rend, float interval);
};

class NameEntry : public LogicModule, MessageHandler<MenuMessage> {
  public:
    std::string name() const noexcept { return mname_; }
    std::string role() const noexcept { return role_; }

    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);

    DELETE_COPY(NameEntry);
    NameEntry(NameEntry&& move) noexcept;
    NameEntry& operator=(NameEntry&& move) noexcept;
    NameEntry(std::shared_ptr<ModuleHolder> holder_,
              const PartialHighScoreEntry& entry, HighScoreTable* table);
    virtual ~NameEntry() noexcept;

  private:
    static inline const std::string mname_ = "NameEntry";
    static inline const std::string role_ = "name entry";

    std::shared_ptr<ModuleHolder> holder_;
    PartialHighScoreEntry entry_;
    HighScoreTable* table_;
    std::array<int, highScoreNameLength> name_;
    int index_{0};
    int rank_{0};
    float t_{0};
    bool running_{true};
    float countdown_{61};
    RendererText rendtext_;
    Renderer2D rend2d_;
    SplinterBuffer maintext_;
    std::array<SplinterBuffer, highScoreNameLength> namebuf_;
    std::array<bool, highScoreNameLength> nameok_{true, false, false};
    LimitedVector<std::unique_ptr<Firework>, 64> fireworks_;

    void redrawChar(size_t i);
};
};  // namespace hiemalia

#endif  // M_GAME_NAMEENTR_HH
