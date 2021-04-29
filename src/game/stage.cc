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
#include "game/enemy.hh"
#include "game/objects.hh"
#include "game/sections.hh"
#include "load3d.hh"
#include "logger.hh"
#include "math.hh"
#include "secure.hh"
#include "str.hh"

namespace hiemalia {
GameStage::GameStage(std::vector<section_t>&& sections, int loopLength,
                     std::deque<ObjectSpawn>&& spawns)
    : spawns_(std::move(spawns)), sections_(std::move(sections)) {
    dynamic_assert(loopLength > 0, "must have loop!");
    auto& s = this->sections_;
    dynamic_assert(loopLength <= static_cast<int>(s.size()), "loop too long!");
    for (int e = s.size(), i = e - loopLength; i < e; ++i)
        sectionsLoop_.push_back(s[i]);
    for (unsigned i = 0; i < stageVisibility; ++i) nextSection();
}

void GameStage::nextSection() {
    if (overridden_) {
        size_t i = overrideIndex_;
        if (i + 1 < overrideSec_.size()) ++overrideIndex_;
        visible_.push_back(overrideSec_[i]);
        return;
    }
    if (inBoss_) {
        visible_.push_back(bossLoop_[inBossIndex_]);
        inBossIndex_ = (inBossIndex_ + 1) % bossLoop_.size();
        return;
    }
    if (nextSection_ >= sections_.size()) {
        visible_.push_back(sectionsLoop_[nextSectionLoop_]);
        nextSectionLoop_ = (nextSectionLoop_ + 1) % sectionsLoop_.size();
    } else
        visible_.push_back(sections_[nextSection_++]);
}

bool GameStage::shouldSpawnNext(unsigned i, coord_t f) const {
    return !spawns_.empty() && spawns_.front().shouldSpawn(i, f);
}

ObjectSpawn GameStage::spawnNext() {
    ObjectSpawn tmp(std::move(spawns_.front()));
    spawns_.pop_front();
    return tmp;
}

coord_t GameStage::getObjectBackPlane(coord_t offset) const {
    int i = -stageSectionOffset;
    for (auto section : visible_) {
        const GameSection& sec = getSectionById(section);
        if (!sec.rotation.isZero()) return i * stageSectionLength - offset;
        ++i;
    }
    return farObjectBackPlane;
}

void GameStage::drawStage(SplinterBuffer& sbuf, Renderer3D& r3d,
                          coord_t offset) {
    int i = stageSectionOffset;
    Point3D p = Point3D(0, 0, stageSectionOffset * stageSectionLength - offset);
    Point3D v = Point3D(0, 0, stageSectionLength);
    Orient3D r = Orient3D(0, 0, 0);
    // Matrix3D m;
    static const Point3D s = Point3D(1, 1, 1);
    for (auto section : visible_) {
        const GameSection& sec = getSectionById(section);
        // m = Matrix3D::rotate(r);
        r3d.renderModel(sbuf, p, r, s, sec.model);
        /*if (i < 0)
            p += v;
        else {
            p += m.project(v);
            r += sec.rotation;
        }*/
        p += v;
        ++i;
    }
}

static MoveRegion parseMoveRegion(std::string s) {
    coord_t x0 = -1, x1 = 1, y0 = -1, y1 = 1;
    if (s_sscanf(s.c_str(),
                 FMT_coord_t " " FMT_coord_t " " FMT_coord_t " " FMT_coord_t,
                 &x0, &x1, &y0, &y1) < 4)
        never("Invalid move region");
    return MoveRegion{x0, x1, y0, y1};
}

static Orient3D parseRotation3D(std::string s) {
    coord_t y = 0, p = 0, r = 0;
    if (s_sscanf(s.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &y, &p,
                 &r) < 3)
        never("Invalid rotation");
    return Orient3D{y, p, r};
}

GameSection loadSection(const std::string& name) {
    std::string filename = name + ".sc";
    LOG_DEBUG("loading stage section data %s", filename);
    auto in = openAssetFileRead("sections", filename, false);
    if (in.fail())
        throw std::runtime_error("cannot open section file " + filename);
    std::string modelFile = "";
    MoveRegion moveRegion = {-1, 1, -1, 1};
    Orient3D turn = {0, 0, 0};

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
        } else if (command == "turn") {
            turn = parseRotation3D(value);
        }
    }

    dynamic_assert(!modelFile.empty(), "must have model file in section");

    return GameSection{load3D("smodels", modelFile), moveRegion, turn};
}

static ObjectSpawn parseObject(std::string v, coord_t& dist, int lineNum,
                               bool front, bool relative) {
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

    coord_t f = stageDivision;
    if (relative)
        f *= dist + r;
    else
        f *= dist, dist += r;

    unsigned u = floatToWholeFrac<unsigned>(f);
    f *= stageSectionLength;
    z += front ? 0 : stageSpawnDistance;
    auto ptr = loadObjectSpawn(Point3D(x, y, z), name, prop);
    return ObjectSpawn{
        ptr, std::dynamic_pointer_cast<EnemyObject>(ptr) != nullptr, u, f};
}

static ObjectSpawn parseObjectZ(std::string v, coord_t& dist, int lineNum) {
    coord_t x, y, z;
    unsigned n, q;
    q = s_sscanf(v.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t "%n",
                 &x, &y, &z, &n);
    if (q < 3)
        throw std::runtime_error("invalid object spawn on line number " +
                                 std::to_string(lineNum));
    std::string name = trimLeft(v.substr(n));
    std::string prop = "";
    size_t i = name.find_first_of(" \t");
    if (i != std::string::npos) {
        prop = name.substr(i + 1);
        name = name.substr(0, i);
    }

    coord_t f = stageDivision * z - stageSpawnDistance;
    unsigned u = floatToWholeFrac<unsigned>(f);
    f *= stageSectionLength;
    z = stageSpawnDistance;
    auto ptr = loadObjectSpawn(Point3D(x, y, z), name, prop);
    return ObjectSpawn{
        ptr, std::dynamic_pointer_cast<EnemyObject>(ptr) != nullptr, u, f};
}

void GameStage::processSectionCommand(std::vector<section_t>& sections,
                                      std::string s) {
    static const decltype(std::string::npos) npos = std::string::npos;
    std::string::size_type prev = 0, next = 0;
    int i;
    section_t sec;
    do {
        next = s.find(' ', prev);
        std::string::size_type mult = s.find('*', prev);
        int times = 1;
        if (mult != npos && (next == npos || mult < next)) {
            times = std::atoi(substring(s, prev, mult).c_str());
            ++mult;
        } else {
            mult = prev;
        }
        sec = getSectionIdByName(substring(s, mult, next));
        for (i = 0; i < times; ++i) sections.push_back(sec);
        prev = next == npos ? next : next + 1;
    } while (prev != npos);
}

GameStage GameStage::load(int stagenum) {
    std::string name = "stage" + std::to_string(stagenum) + ".s";
    LOG_DEBUG("loading stage %s", name);
    auto in = openAssetFileRead("stages", name, false);
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
            processSectionCommand(sections, value);
        } else if (command == "l") {  // l <loopLength>
            loopLength = std::atoi(value.c_str());
        } else if (command == "o") {  // o <reldist> <x> <y> <z> <name> [prop]
            spawns.push_back(parseObject(value, offset, lineNum, false, false));
        } else if (command == "or") {  // or <reldist> <x> <y> <z> <name> [prop]
            spawns.push_back(parseObject(value, offset, lineNum, false, true));
        } else if (command == "x") {  // x <reldist> <x> <y> <z> <name> [prop]
            spawns.push_back(parseObject(value, offset, lineNum, true, false));
        } else if (command == "xr") {  // xr <reldist> <x> <y> <z> <name> [prop]
            spawns.push_back(parseObject(value, offset, lineNum, true, true));
        } else if (command == "z") {  // z <x> <y> <z> <name> [prop]
            spawns.push_back(parseObjectZ(value, offset, lineNum));
        } else if (command == "od") {  // od <reldist>
            offset += fromString<coord_t>(value);
        }
    }

    LOG_TRACE("sections=%d", sections.size());
    LOG_TRACE("offset=" FMT_coord_t, offset);
    std::sort(spawns.begin(), spawns.end(),
              [&](const ObjectSpawn& a, const ObjectSpawn& b) {
                  return a.pos_i < b.pos_i ||
                         (a.pos_i == b.pos_i && a.pos_f < b.pos_f);
              });
    std::deque<ObjectSpawn> realSpawns(spawns.begin(), spawns.end());
    return GameStage(std::move(sections), loopLength, std::move(realSpawns));
}

void GameStage::enterBossLoop(std::initializer_list<section_t> loop) {
    inBoss_ = true;
    bossLoop_ = loop;
    inBossIndex_ = 0;
    dynamic_assert(bossLoop_.size() > 0,
                   "must have at least one section to loop");
}

void GameStage::exitBossLoop() { inBoss_ = false; }

void GameStage::doOverride(std::initializer_list<section_t> sec) {
    overridden_ = true;
    overrideIndex_ = 0;
    overrideSec_ = sec;
}

}  // namespace hiemalia
