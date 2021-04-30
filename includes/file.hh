/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// file.hh: header file for file handling

#ifndef M_FILE_HH
#define M_FILE_HH

#include <fstream>
#include <memory>

namespace hiemalia {
std::ifstream openFileRead(const std::string& filename, bool binary);
std::ofstream openFileWrite(const std::string& filename, bool binary);
std::unique_ptr<std::ofstream> openLogFileWrite(const std::string& filename);
std::ifstream openAssetFileRead(const std::string& filename, bool binary);
std::string buildAssetFilePath(const std::string& folder,
                               const std::string& filename);
std::ifstream openAssetFileRead(const std::string& folder,
                                const std::string& filename, bool binary);
void fileThrowOnError(std::ios& stream);
void fileThrowOnFatalError(std::ios& stream);
void useAlternativeAssetsFolder(const std::string& s);
};  // namespace hiemalia

#endif  // M_FILE_HH
