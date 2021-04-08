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

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "rend2d.hh"
#include "rend3d.hh"
#include "sbuf.hh"
#include "scores.hh"

namespace hiemalia {

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
    float t_{0};
    bool continue_{true};
    RendererText rendtext_;
    SplinterBuffer maintext_;
    std::array<SplinterBuffer, highScoreNameLength> namebuf_;

    void redrawChar(size_t i);
};
};  // namespace hiemalia

#endif  // M_GAME_NAMEENTR_HH
