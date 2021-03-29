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
inline const coord_t stageSectionLength = 1.0 / stageDivision;
constexpr unsigned stageSpawnDistance = 4;

struct ObjectSpawn {
    std::shared_ptr<GameObject> obj;
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
};

GameSection loadSection(const std::string& name);

struct GameStage {
    std::vector<section_t> sections;
    std::vector<section_t> sectionsLoop;
    std::deque<ObjectSpawn> spawns;
    circular_buffer<section_t, stageVisibility> visible;

    void loadStage();
    void nextSection();
    void drawStage(SplinterBuffer& sbuf, Renderer3D& r3d, coord_t offset);
    static GameStage load(int stagenum);

   private:
    GameStage(std::vector<section_t>&& sections, int loopLength,
              std::deque<ObjectSpawn>&& spawns);
    size_t nextSection_{0};
    size_t nextSectionLoop_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_STAGE_HH
