/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/stage.cc: implementation of GameStage

#include "game/stage.hh"

#include <cstdlib>

#include "assets.hh"
#include "collide.hh"
#include "file.hh"
#include "game/objects.hh"
#include "game/sections.hh"
#include "load3d.hh"
#include "logger.hh"
#include "math.hh"
#include "secure.hh"
#include "str.hh"

namespace hiemalia {
static MoveRegion parseMoveRegion(std::string s) {
    coord_t x0 = -1, x1 = 1, y0 = -1, y1 = 1;
    if (s_sscanf(s.c_str(),
                 FMT_coord_t " " FMT_coord_t " " FMT_coord_t " " FMT_coord_t,
                 &x0, &x1, &y0, &y1) < 4)
        never("Invalid move region");
    return MoveRegion{x0, x1, y0, y1};
}

GameSection loadSection(const std::string& name) {
    std::string filename = name + ".sc";
    LOG_DEBUG("loading stage section data %s", filename);
    std::ifstream in = openAssetFileRead("sections", filename, false);
    if (in.fail())
        throw std::runtime_error("cannot open section file " + filename);
    std::string modelFile = "";
    MoveRegion moveRegion = {-1, 1, -1, 1};

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "model") {
            modelFile = value;
        } else if (command == "move") {
            moveRegion = parseMoveRegion(value);
        }
    }

    dynamic_assert(!modelFile.empty(), "must have model file in section");

    return GameSection{load3D("smodels", modelFile), moveRegion};
}

void GameStage::nextSection() {
    if (nextSection_ >= sections.size()) {
        visible.push_back(sectionsLoop[nextSectionLoop_]);
        nextSectionLoop_ = (nextSectionLoop_ + 1) % sectionsLoop.size();
    } else
        visible.push_back(sections[nextSection_++]);
}

void GameStage::drawStage(SplinterBuffer& sbuf, Renderer3D& r3d,
                          coord_t offset) {
    int i = stageSectionOffset;
    static const Rotation3D r = Rotation3D(0, 0, 0);
    static const ModelPoint s = ModelPoint(1, 1, 1);
    for (auto section : visible)
        r3d.renderModel(sbuf,
                        ModelPoint(0, 0, i++ * stageSectionLength - offset), r,
                        s, getSectionById(section).model);
}

GameStage::GameStage(std::vector<section_t>&& sections, int loopLength,
                     std::deque<ObjectSpawn>&& spawns)
    : sections(std::move(sections)), spawns(std::move(spawns)) {
    dynamic_assert(loopLength > 0, "must have loop!");
    auto& s = this->sections;
    dynamic_assert(loopLength >= static_cast<int>(s.size()), "loop too long!");
    for (int e = s.size(), i = e - loopLength; i < e; ++i)
        sectionsLoop.push_back(s[i]);
    for (unsigned i = 0; i < stageVisibility; ++i) nextSection();
}

static ObjectSpawn parseObject(std::string v, coord_t dist, int lineNum) {
    coord_t r, x, y, z;
    unsigned n, q;
    q = s_sscanf(v.c_str(),
                 FMT_coord_t " " FMT_coord_t " " FMT_coord_t " " FMT_coord_t
                             "%n",
                 &r, &x, &y, &z, &n);
    if (q < 4)
        throw std::runtime_error("invalid object spawn on line number " +
                                 std::to_string(lineNum));
    std::string name = trimLeft(v.substr(n));
    std::string prop = "";
    size_t i = name.find_first_of(" \t");
    if (i != std::string::npos) {
        prop = name.substr(i + 1);
        name = name.substr(0, i);
    }

    coord_t f = dist + r;

    unsigned u = floatToWholeFrac<unsigned>(f);
    f *= stageSectionLength;
    z += stageSpawnDistance;
    return ObjectSpawn{loadObjectSpawn(ModelPoint(x, y, z), name, prop), u, f};
}

GameStage GameStage::load(int stagenum) {
    std::string name = "stage" + std::to_string(stagenum) + ".s";
    LOG_DEBUG("loading stage %s", name);
    std::ifstream in = openAssetFileRead("stages", name, false);
    if (in.fail()) throw std::runtime_error("cannot open stage file " + name);
    std::vector<section_t> sections;
    int loopLength = 1, lineNum = 0;
    std::vector<ObjectSpawn> spawns;
    coord_t offset = 0;

    for (std::string line; std::getline(in, line);) {
        ++lineNum;
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "s") {  // s <section>
            sections.push_back(getSectionIdByName(value));
        } else if (command == "l") {  // l <loopLength>
            loopLength = std::atoi(value.c_str());
        } else if (command == "o") {  // o <reldist> <x> <y> <z> <name> [prop]
            spawns.push_back(parseObject(value, offset, lineNum));
        } else if (command == "od") {  // od <reldist>
            offset += fromString<coord_t>(value);
        }
    }

    std::sort(spawns.begin(), spawns.end(),
              [&](const ObjectSpawn& a, const ObjectSpawn& b) {
                  return a.pos_i < b.pos_i ||
                         (a.pos_i == b.pos_i && a.pos_f < b.pos_f);
              });
    std::deque<ObjectSpawn> realSpawns(spawns.begin(), spawns.end());
    return GameStage(std::move(sections), loopLength, std::move(realSpawns));
}

}  // namespace hiemalia
