/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// config.hh: header file for configuration

#ifndef M_CONFIG_HH
#define M_CONFIG_HH

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "str.hh"

namespace hiemalia {

class ConfigStore {
  public:
    void validate_key(const std::string& key) const {
        if (key.find('=') != std::string::npos)
            throw std::invalid_argument("key");
    }

    template <typename T>
    bool has(const std::string& key) const {
        validate_key(key);
        typename decltype(map_)::const_iterator it = map_.find(key);
        if (it == map_.end()) return false;

        return canFromString<T>(it->second);
    }

    template <typename T>
    T get(const std::string& key) const {
        validate_key(key);
        return get<T>(key, {});
    }

    template <typename T>
    T get(const std::string& key, T def) const {
        validate_key(key);
        typename decltype(map_)::const_iterator it = map_.find(key);
        if (it == map_.end() || !canFromString<T>(it->second)) return def;

        return fromString<T>(it->second);
    }

    template <typename T>
    void set(const std::string& key, T value) {
        validate_key(key);
        map_.insert_or_assign(key, toString<T>(value));
    }

    void clear() noexcept;
    void readStream(std::istream& stream);
    void writeStream(std::ostream& stream) const;

  private:
    std::map<std::string, std::string> map_;
};

class ConfigSectionStore;

class ConfigSection {
  public:
    virtual void load(ConfigSectionStore store) = 0;
    virtual void save(ConfigSectionStore store) const = 0;
    const std::string name;
    virtual ~ConfigSection() {}

  protected:
    ConfigSection(std::string name) : name(name) {}
};

class ConfigSectionStore {
  public:
    template <typename T>
    bool has(const std::string& key) const {
        return store_.has<T>(header_ + key);
    }

    template <typename T>
    T get(const std::string& key) const {
        return store_.get<T>(header_ + key);
    }

    template <typename T>
    T get(const std::string& key, T def) const {
        return store_.get<T>(header_ + key, def);
    }

    template <typename T>
    void set(const std::string& key, T value) {
        store_.set<T>(header_ + key, value);
    }

    ConfigSectionStore(const ConfigSection& section, ConfigStore& store)
        : header_(section.name + ":"), store_(store) {}

  private:
    const std::string header_;
    ConfigStore& store_;
};

template <typename T>
using ConfigSectionPtr = std::shared_ptr<T>;
using ConfigSectionPtrDefault = ConfigSectionPtr<ConfigSection>;

class Config {
  public:
    template <typename T>
    using ConfigSectionContainer = std::vector<T>;

    void load(std::string filename);
    void save(std::string filename);

    template <typename T, typename... Ts>
    ConfigSectionPtr<T> section(Ts&&... args) {
        auto sptr = std::make_shared<T>(std::forward<Ts>(args)...);
        sectionLoad(sptr);
        return std::dynamic_pointer_cast<T>(confs_.emplace_back(sptr));
    }

    template <typename T>
    void sectionLoad(const T& section) {
        section->load(ConfigSectionStore(*section, store_));
    }

    template <typename T>
    void sectionSave(const T& section) {
        section->save(ConfigSectionStore(*section, store_));
    }

  private:
    ConfigStore store_;
    ConfigSectionContainer<ConfigSectionPtrDefault> confs_;
};
};  // namespace hiemalia

#endif  // M_CONFIG_HH
