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
#include "game/checkpnt.hh"
#include "game/mbox.hh"
#include "game/object.hh"
#include "game/stageend.hh"
#include "secure.hh"

namespace hiemalia {
template <typename T>
std::shared_ptr<GameObject> makeStandardObject(const ModelPoint& p,
                                               const std::string& prop) {
    auto s = std::make_shared<T>();
    s->move(p);
    return s;
}

template <typename T>
std::shared_ptr<GameObject> makeScalableObject(const ModelPoint& p,
                                               const std::string& prop) {
    coord_t sx = 1, sy = 1, sz = 1;
    if (s_sscanf(prop.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &sx,
                 &sy, &sz) < 3)
        LOG_WARN("invalid scalable object prop");
    auto s = std::make_shared<T>(sx, sy, sz);
    s->move(p);
    return s;
}

typedef std::shared_ptr<GameObject> (*object_maker_t)(const ModelPoint& p,
                                                      const std::string& prop);

static const std::unordered_map<std::string, object_maker_t> nameMap = {
    {"box", makeStandardObject<Box>},
    {"mbox", makeScalableObject<ScalableBox>},
    {"checkpoint", makeStandardObject<CheckpointScript>},
    {"end", makeStandardObject<StageEndScript>},
};

std::shared_ptr<GameObject> loadObjectSpawn(ModelPoint p,
                                            const std::string& name,
                                            const std::string& prop) {
    auto it = nameMap.find(name);
    if (it == nameMap.end()) return nullptr;
    return (*it).second(p, prop);
}

}  // namespace hiemalia
