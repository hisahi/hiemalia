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
#include "game/sections.hh"
#include "load3d.hh"
#include "logger.hh"

namespace hiemalia {
static MoveRegion parseMoveRegion(std::string s) {
    coord_t x0 = -1, x1 = 1, y0 = -1, y1 = 1;
    std::sscanf(s.c_str(),
                FMT_coord_t " " FMT_coord_t " " FMT_coord_t " " FMT_coord_t,
                &x0, &x1, &y0, &y1);
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
    int i = -2;
    for (auto section : visible)
        r3d.renderModel(sbuf,
                        ModelPoint(0, 0, i++ * stageSectionLength - offset),
                        Rotation3D(0, 0, 0), 1, getSectionById(section).model);
}

GameStage::GameStage(std::vector<section_t>&& sections, int loopLength,
                     std::vector<ObjectSpawn>&& spawns)
    : sections(std::move(sections)), spawns(std::move(spawns)) {
    dynamic_assert(loopLength > 0, "must have loop!");
    auto& s = this->sections;
    dynamic_assert(loopLength >= static_cast<int>(s.size()), "loop too long!");
    for (int e = s.size(), i = e - loopLength; i < e; ++i)
        sectionsLoop.push_back(s[i]);
    for (unsigned i = 0; i < stageVisibility; ++i) nextSection();
}

GameStage GameStage::load(int stagenum) {
    std::string name = "stage" + std::to_string(stagenum) + ".s";
    LOG_DEBUG("loading stage %s", name);
    std::ifstream in = openAssetFileRead("stages", name, false);
    if (in.fail()) throw std::runtime_error("cannot open stage file " + name);
    std::vector<section_t> sections;
    int loopLength = 1;
    std::vector<ObjectSpawn> spawns;

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "s") {
            sections.push_back(getSectionIdByName(value));
        } else if (command == "l") {
            loopLength = std::atoi(value.c_str());
        }
    }

    return GameStage(std::move(sections), loopLength, std::move(spawns));
}

}  // namespace hiemalia
