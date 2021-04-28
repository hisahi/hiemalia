/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// config.cc: implementation of ConfigStore

#include "config.hh"

#include "file.hh"
#include "logger.hh"

namespace hiemalia {
void ConfigStore::clear() noexcept { map_.clear(); }

void ConfigStore::readStream(std::istream& stream) {
    for (std::string line; std::getline(stream, line);) {
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
    LOG_TRACE("loading config from %s", filename);
    auto f = openFileRead(filename, false);
    if (f.fail()) {
        LOG_ERROR("failed to read config file %s. will use defaults.",
                  filename);
        return;
    }
    fileThrowOnFatalError(f);
    store_.readStream(f);
    for (auto& conf : confs_) conf->load(ConfigSectionStore(*conf, store_));
}

void Config::save(std::string filename) {
    for (auto& conf : confs_) conf->save(ConfigSectionStore(*conf, store_));
    auto f = openFileWrite(filename, false);
    if (f.fail()) {
        LOG_ERROR(
            "cannot open config file for writing. will dump into cout "
            "instead.");
        store_.writeStream(std::cout);
        return;
    }
    store_.writeStream(f);
}
}  // namespace hiemalia
