/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// file.cc: implementation of file system

#include "file.hh"

#include "logger.hh"

namespace hiemalia {
static std::string assetsFolder = "assets";

void useAlternativeAssetsFolder(const std::string& s) { assetsFolder = s; }

std::ifstream openFileRead(const std::string& filename, bool binary) {
    return std::ifstream(
        filename, binary ? std::ios::in | std::ios::binary : std::ios::in);
}

std::ofstream openFileWrite(const std::string& filename, bool binary) {
    return std::ofstream(
        filename, binary ? std::ios::out | std::ios::binary : std::ios::out);
}

std::unique_ptr<std::ofstream> openLogFileWrite(const std::string& filename) {
    auto of = std::ofstream(filename, std::ios::out);
    if (of.fail()) {
        LOG_WARN("cannot open log file '" + filename + "'");
        return nullptr;
    }
    return std::make_unique<std::ofstream>(std::move(of));
}

std::ifstream openAssetFileRead(const std::string& filename, bool binary) {
    return std::ifstream(
        assetsFolder + "/" + filename,
        binary ? std::ios::in | std::ios::binary : std::ios::in);
}

std::string buildAssetFilePath(const std::string& folder,
                               const std::string& filename) {
    return assetsFolder + "/" + folder + "/" + filename;
}

std::ifstream openAssetFileRead(const std::string& folder,
                                const std::string& filename, bool binary) {
    return std::ifstream(
        assetsFolder + "/" + folder + "/" + filename,
        binary ? std::ios::in | std::ios::binary : std::ios::in);
}

void fileThrowOnError(std::ios& stream) {
    stream.exceptions(std::ios::failbit | std::ios::badbit);
}

void fileThrowOnFatalError(std::ios& stream) {
    stream.exceptions(std::ios::badbit);
}
}  // namespace hiemalia
