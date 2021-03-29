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

namespace hiemalia {
std::ifstream openFileRead(const std::string& filename, bool binary);
std::ofstream openFileWrite(const std::string& filename, bool binary);
std::ifstream openAssetFileRead(const std::string& filename, bool binary);
std::string buildAssetFilePath(const std::string& folder,
                               const std::string& filename);
std::ifstream openAssetFileRead(const std::string& folder,
                                const std::string& filename, bool binary);
};  // namespace hiemalia

#endif  // M_FILE_HH
