/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/stage.hh: header file for game stages (game/stage.cc)

#ifndef M_GAME_STAGE_HH
#define M_GAME_STAGE_HH

#include <deque>
#include <vector>

#include "cbuffer.hh"
#include "game/object.hh"
#include "model.hh"
#include "rend3d.hh"
#include "sbuf.hh"

namespace hiemalia {
using section_t = unsigned;
constexpr unsigned stageVisibility = 22;
constexpr unsigned stageDivision = 4;
constexpr int stageSectionOffset = -2;
inline const coord_t stageSectionLength = 1.0 / stageDivision;
constexpr unsigned stageSpawnDistance = 5;

struct ObjectSpawn {
    std::shared_ptr<GameObject> obj;
    bool isEnemy;
    unsigned pos_i;
    coord_t pos_f;

    inline bool shouldSpawn(unsigned i, coord_t f) const {
        return i > pos_i || (i == pos_i && f >= pos_f);
    }
};

struct MoveRegion {
    coord_t x0, x1;
    coord_t y0, y1;
};

struct GameSection {
    Model model;
    MoveRegion region;
    Orient3D rotation;
};

GameSection loadSection(const std::string& name);

class GameStage {
  public:
    using visible_type = CircularBuffer<section_t, stageVisibility>;

    void nextSection();
    void drawStage(SplinterBuffer& sbuf, Renderer3D& r3d, coord_t offset);
    coord_t getObjectBackPlane(coord_t offset) const;
    static GameStage load(int stagenum);
    void enterBossLoop(std::initializer_list<section_t> loop);
    void exitBossLoop();
    void doOverride(std::initializer_list<section_t> sec);
    inline const visible_type visible() const noexcept { return visible_; }
    bool shouldSpawnNext(unsigned i, coord_t f) const;
    ObjectSpawn spawnNext();

  private:
    GameStage(std::vector<section_t>&& sections, int loopLength,
              std::deque<ObjectSpawn>&& spawns);
    static void processSectionCommand(std::vector<section_t>& sections,
                                      const std::string& s);
    std::deque<ObjectSpawn> spawns_;
    visible_type visible_;
    std::vector<section_t> sections_;
    std::vector<section_t> sectionsLoop_;
    size_t nextSection_{0};
    size_t nextSectionLoop_{0};
    bool inBoss_{false};
    size_t inBossIndex_{0};
    std::vector<section_t> bossLoop_;
    bool overridden_{false};
    size_t overrideIndex_{0};
    std::vector<section_t> overrideSec_;
};
};  // namespace hiemalia

#endif  // M_GAME_STAGE_HH
