/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// file.cc: implementation of file system

#include "file.hh"

namespace hiemalia {
std::ifstream openFileRead(const std::string& filename, bool binary) {
    return std::move(std::ifstream(
        filename, binary ? std::ios::in | std::ios::binary : std::ios::in));
}

std::ofstream openFileWrite(const std::string& filename, bool binary) {
    return std::move(std::ofstream(
        filename, binary ? std::ios::out | std::ios::binary : std::ios::out));
}
}  // namespace hiemalia
