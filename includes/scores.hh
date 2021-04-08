/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// scores.hh: header file for high score functionality (scores.cc)

#ifndef M_SCORES_HH
#define M_SCORES_HH

#include <vector>

#include "array.hh"
#include "defs.hh"

namespace hiemalia {

inline const auto validScoreNameChars =
    hiemalia::strToArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-");
constexpr int highScoreNameLength = 3;

struct HighScoreEntry {
    std::array<char, highScoreNameLength> name;
    unsigned long score;
    int cycles;
    int endStage;
};
struct PartialHighScoreEntry {
    unsigned long score;
    int cycles;
    int endStage;
};

struct HighScoreTable {
    static constexpr int size = 10;
    std::vector<HighScoreEntry> entries;

    int getHighscoreRank(unsigned long score) const;
    int insertHighscore(HighScoreEntry&& entry);
    void resetScores();
    unsigned long getTopScore() const;
};

HighScoreTable loadHighscores();
void saveHighscores(HighScoreTable& table);
};  // namespace hiemalia

#endif  // M_SCORES_HH
