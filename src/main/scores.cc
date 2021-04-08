/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/scores.cc: implementation of high score functionality

#include "scores.hh"

#include <algorithm>

#include "array.hh"
#include "file.hh"
#include "game/world.hh"
#include "logger.hh"
#include "mstream.hh"
#include "random.hh"
#include "sort.hh"

namespace hiemalia {
static const std::string scoresFilename = "scores.dat";
static const auto randomChars =
    hiemalia::strToArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

int HighScoreTable::getHighscoreRank(unsigned long score) const {
    if (!score) return -1;
    int i = 0;
    for (const HighScoreEntry& entry : entries) {
        if (score > entry.score) {
            break;
        }
        ++i;
    }
    return i >= HighScoreTable::size ? -1 : i;
}

int HighScoreTable::insertHighscore(HighScoreEntry&& entry) {
    dynamic_assert(arrayContains(validScoreNameChars, entry.name[0]),
                   "invalid character in high score!");
    dynamic_assert(arrayContains(validScoreNameChars, entry.name[1]),
                   "invalid character in high score!");
    dynamic_assert(arrayContains(validScoreNameChars, entry.name[2]),
                   "invalid character in high score!");
    entries.push_back(std::move(entry));
    auto idx = stableIndexSort(
        entries.begin(), entries.end(),
        [&](const HighScoreEntry& lhs, const HighScoreEntry& rhs) {
            return lhs.score > rhs.score;
        });
    if (entries.size() > HighScoreTable::size)
        entries.erase(entries.begin() + HighScoreTable::size, entries.end());
    else
        never("table is cut off");
    return static_cast<int>(
        std::find(idx.begin(), idx.end(), HighScoreTable::size) - idx.begin());
}

unsigned long HighScoreTable::getTopScore() const {
    if (entries.empty()) return 0;
    return entries.front().score;
}

static HighScoreTable defaultScores;

static HighScoreTable getDefaultHighscoreTable() {
    if (defaultScores.entries.empty()) {
        auto& e = defaultScores.entries;
        std::uniform_int_distribution<int> rc(0, randomChars.size() - 1);
        for (int i = 0; i < HighScoreTable::size; ++i) {
            e.push_back(HighScoreEntry{
                {randomChars[random(rc)], randomChars[random(rc)],
                 randomChars[random(rc)]},
                (i + 1) * 2500UL,
                1 + (i / 2) / stageCount,
                ((i / 2) % stageCount) + 1});
        }
        std::reverse(e.begin(), e.end());
    }
    return defaultScores;
}

void HighScoreTable::resetScores() {
    defaultScores.entries.clear();
    *this = getDefaultHighscoreTable();
}

static char readChar(std::istream& s) { return s.get(); }

static uint16_t readUInt16(std::istream& s) {
    unsigned char buf[2];
    s.read(reinterpret_cast<char*>(buf), 2);
    return static_cast<uint16_t>(buf[0] | (static_cast<uint16_t>(buf[1]) << 8));
}

static int32_t readInt32(std::istream& s) {
    unsigned char buf[4];
    s.read(reinterpret_cast<char*>(buf), 4);
    return static_cast<int32_t>(static_cast<uint32_t>(buf[0]) |
                                (static_cast<uint32_t>(buf[1]) << 8) |
                                (static_cast<uint32_t>(buf[2]) << 16) |
                                (static_cast<uint32_t>(buf[3]) << 24));
}

static uint64_t readUInt64(std::istream& s) {
    unsigned char buf[8];
    s.read(reinterpret_cast<char*>(buf), 8);
    return static_cast<uint64_t>(static_cast<uint64_t>(buf[0]) |
                                 (static_cast<uint64_t>(buf[1]) << 8) |
                                 (static_cast<uint64_t>(buf[2]) << 16) |
                                 (static_cast<uint64_t>(buf[3]) << 24) |
                                 (static_cast<uint64_t>(buf[4]) << 32) |
                                 (static_cast<uint64_t>(buf[5]) << 40) |
                                 (static_cast<uint64_t>(buf[6]) << 48) |
                                 (static_cast<uint64_t>(buf[7]) << 56));
}

static HighScoreEntry readHighscoreEntry(std::istream& s) {
    std::array<char, highScoreNameLength> n;
    for (int i = 0; i < highScoreNameLength; ++i) n[i] = readChar(s);
    return HighScoreEntry{
        n, static_cast<unsigned long>((readChar(s), readUInt64(s))),
        static_cast<int>(readUInt16(s)), static_cast<int>(readChar(s))};
}

static void writeChar(std::ostream& s, char c) { s.put(c); }

static void writeUInt16(std::ostream& s, uint16_t v) {
    s.put(v & 0xFF);
    s.put((v >> 8) & 0xFF);
}

static void writeInt32(std::ostream& s, int32_t v) {
    s.put(v & 0xFFU);
    s.put((v >> 8) & 0xFFU);
    s.put((v >> 16) & 0xFFU);
    s.put((v >> 24) & 0xFFU);
}

static void writeUInt64(std::ostream& s, uint64_t v) {
    s.put(v & 0xFFU);
    s.put((v >> 8) & 0xFFU);
    s.put((v >> 16) & 0xFFU);
    s.put((v >> 24) & 0xFFU);
    s.put((v >> 32) & 0xFFU);
    s.put((v >> 40) & 0xFFU);
    s.put((v >> 48) & 0xFFU);
    s.put((v >> 56) & 0xFFU);
}

static void writeHighscoreEntry(std::ostream& s, const HighScoreEntry& h) {
    for (int i = 0; i < highScoreNameLength; ++i) writeChar(s, h.name[i]);
    writeChar(s, 0x27);
    writeUInt64(s, static_cast<uint64_t>(h.score));
    writeUInt16(s, static_cast<uint16_t>(h.cycles));
    writeChar(s, static_cast<char>(h.endStage));
}

static char crypt_next(int32_t& s) {
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return static_cast<char>(static_cast<int32_t>(s * 0x951d71f1UL) >> 9);
}

static int32_t checksum(const char* p, const char* e) {
    uint32_t s = 0x55555555;
    while (p < e) {
        s = (s << 9) | (s >> 23);
        s ^= (static_cast<unsigned char>(*p++) * 0x101) ^ 77;
        s = (s << 17) | (s >> 15);
    }
    return static_cast<int32_t>(s);
}

static void crypt(char* p, char* e, int32_t k) {
    while (p < e) *p++ ^= crypt_next(k);
}

HighScoreTable loadHighscores() {
    auto in = openFileRead(scoresFilename, true);
    if (in.fail()) {
        LOG_WARN("cannot open high scores, using defaults");
        return getDefaultHighscoreTable();
    }
    in.exceptions(std::ifstream::failbit);

    HighScoreTable table;
    try {
        int32_t checksum_unenc = readInt32(in);
        int32_t key = readInt32(in);
        imemorystream memstr(in);
        int32_t a = checksum(memstr.ibegin(), memstr.iend());
        crypt(memstr.ibegin(), memstr.iend(), key);
        int32_t checksum_enc = readInt32(memstr);
        int32_t b = checksum(memstr.ibegin(), memstr.iend());
        if (a != checksum_unenc || b != checksum_enc) {
            LOG_TRACE("tampered!");
            return getDefaultHighscoreTable();
        }
        for (uint16_t i = 0, n = readUInt16(memstr); i < n; ++i) {
            if (i >= HighScoreTable::size) return getDefaultHighscoreTable();
            table.entries.push_back(readHighscoreEntry(memstr));
            if (i > 1 &&
                table.entries[i - 2].score < table.entries[i - 1].score)
                return getDefaultHighscoreTable();
        }
        if (table.entries.size() != HighScoreTable::size)
            return getDefaultHighscoreTable();
    } catch (std::ios_base::failure&) {
        LOG_WARN("cannot open high scores, using defaults");
        return getDefaultHighscoreTable();
    }
    return table;
}

static void dumpScores(const HighScoreTable& table) {
    LOG_ERROR("Could not save high scores. they were");
    uint16_t n = static_cast<uint16_t>(table.entries.size());
    for (uint16_t i = 0; i < n; ++i) {
        const HighScoreEntry& e = table.entries[i];
        LOG_ERROR("#%2d   %.3s   %2d   %d   %12lu", i + 1, e.name, e.cycles,
                  e.endStage, e.score);
    }
}

void saveHighscores(HighScoreTable& table) {
    if (table.entries.empty()) return;
    auto out = openFileWrite(scoresFilename, true);
    if (out.fail()) {
        dumpScores(table);
    } else {
        try {
            out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            omemorystream maindata;
            uint16_t n = static_cast<uint16_t>(table.entries.size());
            writeUInt16(maindata, n);
            for (uint16_t i = 0; i < n; ++i) {
                writeHighscoreEntry(maindata, table.entries[i]);
            }
            int32_t checksum_enc = checksum(maindata.obegin(), maindata.oend());
            std::uniform_int_distribution<int32_t> rc(INT32_MIN, INT32_MAX);
            int32_t key = random(rc);
            omemorystream crypting;
            writeInt32(crypting, checksum_enc);
            maindata.copy_to(crypting);
            maindata.clear();
            crypt(crypting.obegin(), crypting.oend(), key);
            int32_t checksum_unenc =
                checksum(crypting.obegin(), crypting.oend());
            writeInt32(out, checksum_unenc);
            writeInt32(out, key);
            crypting.copy_to(out);
        } catch (std::ostream::failure&) {
            dumpScores(table);
            throw;
        }
    }
}
}  // namespace hiemalia
