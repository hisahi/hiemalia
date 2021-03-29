/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/objects.cc: implementation of handling object spawns

#include "game/objects.hh"

#include <unordered_map>

#include "game/box.hh"
#include "game/object.hh"

namespace hiemalia {
template <typename T>
std::shared_ptr<GameObject> makeStandardObject(const ModelPoint& p,
                                               const std::string& prop) {
    auto s = std::make_shared<T>();
    s->move(p);
    return s;
}

typedef std::shared_ptr<GameObject> (*object_maker_t)(const ModelPoint& p,
                                                      const std::string& prop);

static const std::unordered_map<std::string, object_maker_t> nameMap = {
    {"box", makeStandardObject<Box>},
};

std::shared_ptr<GameObject> loadObjectSpawn(ModelPoint p,
                                            const std::string& name,
                                            const std::string& prop) {
    auto it = nameMap.find(name);
    if (it == nameMap.end()) return nullptr;
    return (*it).second(p, prop);
}

}  // namespace hiemalia
