/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/sections.cc: implementation of game section resolver

#include "game/sections.hh"

#include "defs.hh"

namespace hiemalia {

const std::unordered_map<std::string, section_t> sectionMap = {
    {"plain", 0},     {"plaintr", 1},   {"cave00", 2},    {"cave01", 3},
    {"cave02", 4},    {"cave03", 5},    {"cave04", 6},    {"cave05", 7},
    {"cave06", 8},    {"cave07", 9},    {"cave08", 10},   {"cave09", 11},
    {"cave10", 12},   {"cave11", 13},   {"cave12", 14},   {"cave13", 15},
    {"cave14", 16},   {"cave15", 17},   {"cave16", 18},   {"corrid00", 19},
    {"corrid01", 20}, {"corrid02", 21}, {"corrid03", 22}, {"corrid04", 23},
    {"corrid05", 24}, {"corrid06", 25}, {"corrid07", 26}, {"room0", 27},
    {"room1", 28},    {"room2", 29},    {"room3", 30},    {"tech00", 31},
    {"tech01", 32},   {"tech02", 33},   {"tech03", 34},   {"tech04", 35},
    {"tech05", 36},   {"tech06", 37},   {"tech07", 38},   {"boss00", 39},
    {"boss01", 40},   {"boss02", 41},   {"boss03", 42},   {"boss04", 43},
    {"boss05", 44},   {"boss06", 45},   {"boss07", 46},   {"fac00", 47},
    {"fac01", 48},    {"fac02", 49},    {"fac03", 50},    {"fac04", 51},
    {"zoomentr", 52}, {"zoom00", 53},   {"zoom01", 54},   {"zoom02", 55},
    {"zoom03", 56},   {"zoom04", 57},   {"zoomexit", 58}, {"sb4", 59},
    {"sb4ent", 60},   {"sb4ext", 61},
};

int getSectionCount() {
    int max = 0;
    for (const auto& it : sectionMap) {
        int n = static_cast<int>(it.second);
        if (max < n) max = n;
    }
    return max;
}

section_t getSectionIdByName(const std::string& s) {
    auto it = sectionMap.find(s);
    if (it == sectionMap.end()) never("unrecognized section name");
    return it->second;
}

}  // namespace hiemalia
