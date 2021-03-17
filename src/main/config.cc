/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// config.cc: implementation of ConfigStore

#include "config.hh"

#include "file.hh"

namespace hiemalia {
void ConfigStore::clear() noexcept { map_.clear(); }

void ConfigStore::readStream(std::istream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        std::size_t i = line.find('=');
        if (i != std::string::npos)
            map_.insert_or_assign(line.substr(0, i), line.substr(i + 1));
    }
}

void ConfigStore::writeStream(std::ostream& stream) const {
    for (auto& pair : map_)
        stream << pair.first << '=' << pair.second << std::endl;
}

void Config::load(std::string filename) {
    store_.clear();
    std::ifstream f = std::move(openFileRead(filename, false));
    store_.readStream(f);
    for (auto& conf : confs_) conf->load(ConfigSectionStore(*conf, store_));
}

void Config::save(std::string filename) {
    for (auto& conf : confs_) conf->save(ConfigSectionStore(*conf, store_));
    std::ofstream f = std::move(openFileWrite(filename, false));
    store_.writeStream(f);
}
}  // namespace hiemalia
