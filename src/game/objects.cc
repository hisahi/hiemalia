/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/objects.cc: implementation of handling object spawns

#include "game/objects.hh"

#include <any>
#include <array>
#include <functional>
#include <istream>
#include <sstream>
#include <unordered_map>

#include "game/box.hh"
#include "game/checkpnt.hh"
#include "game/enemy/all.hh"
#include "game/gameend.hh"
#include "game/mbox.hh"
#include "game/object.hh"
#include "game/obstacle.hh"
#include "game/sbox.hh"
#include "game/setspeed.hh"
#include "game/stageend.hh"
#include "secure.hh"

namespace hiemalia {
template <typename T>
std::shared_ptr<GameObject> makeStandardObject(const Point3D& p,
                                               const std::string& prop) {
    return std::make_shared<T>(p);
}

template <typename T>
std::shared_ptr<GameObject> makePropObject(const Point3D& p,
                                           const std::string& prop) {
    return std::make_shared<T>(p, prop);
}

template <typename T>
std::shared_ptr<GameObject> makeTwoPosObject(const Point3D& p,
                                             const std::string& prop) {
    coord_t sx = 0, sy = 0, sz = 0;
    if (s_sscanf(prop.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &sx,
                 &sy, &sz) < 3)
        LOG_WARN("invalid angled object prop");
    return std::make_shared<T>(p, Point3D(sx, sy, sz));
}

template <typename T>
std::shared_ptr<GameObject> makeAngleObject(const Point3D& p,
                                            const std::string& prop) {
    coord_t sx = 0, sy = 0, sz = 0;
    if (s_sscanf(prop.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &sx,
                 &sy, &sz) < 3)
        LOG_WARN("invalid angled object prop");
    return std::make_shared<T>(p, Orient3D(sx, sy, sz));
}

template <typename T>
std::shared_ptr<GameObject> makeAngleVelObject(const Point3D& p,
                                               const std::string& prop) {
    coord_t sx1 = 0, sy1 = 0, sz1 = 0;
    coord_t sx2 = 0, sy2 = 0, sz2 = 0;
    if (s_sscanf(prop.c_str(),
                 FMT_coord_t " " FMT_coord_t " " FMT_coord_t " " FMT_coord_t
                             " " FMT_coord_t " " FMT_coord_t,
                 &sx1, &sy1, &sz1, &sx2, &sy2, &sz2) < 6)
        LOG_WARN("invalid angled object prop");
    return std::make_shared<T>(p, Orient3D(sx1, sy1, sz1),
                               Orient3D(sx2, sy2, sz2));
}

namespace {
template <typename Ta>
void streamExtract_(std::istream& ss, Ta& out) {
    dynamic_assert(static_cast<bool>(ss >> out),
                   "not enough parameters for object");
}

template <typename Ta, typename Td>
void streamExtractCopy_(std::istream& ss, Td& out) {
    Ta val;
    dynamic_assert(static_cast<bool>(ss >> val),
                   "not enough parameters for object");
    out = std::move(val);
}

template <typename T, typename Ta, std::size_t... I>
std::shared_ptr<GameObject> makeCoordPropObject_(const Point3D& p,
                                                 std::string prop,
                                                 std::index_sequence<I...>) {
    std::array<Ta, sizeof...(I)> arr;
    std::istringstream ss{prop};
    (streamExtract_<Ta>(ss, arr[I]), ...);
    return std::make_shared<T>(p, arr[I]...);
}

template <typename T, typename... Ts, std::size_t... I>
std::shared_ptr<GameObject> makeArgPropObject_(
    const Point3D& p, std::array<std::any, sizeof...(I)>& v,
    const std::string& prop, std::index_sequence<I...>) {
    // contrived^2
    using TTuple = std::tuple<Ts...>;
    std::istringstream ss{prop};
    (streamExtractCopy_<typename std::tuple_element<I, TTuple>::type>(ss, v[I]),
     ...);
    return std::make_shared<T>(
        p,
        std::any_cast<typename std::tuple_element<I, TTuple>::type>(v[I])...);
}
}  // namespace

template <typename T, std::size_t N>
std::shared_ptr<GameObject> makeCoordPropObject(const Point3D& p,
                                                const std::string& prop) {
    using I = std::make_index_sequence<N>;
    return makeCoordPropObject_<T, coord_t>(p, prop, I{});
}

template <typename T, typename... Ts>
std::shared_ptr<GameObject> makeArgPropObject(const Point3D& p,
                                              const std::string& prop) {
    // contrived
    using I = std::make_index_sequence<sizeof...(Ts)>;
    std::array<std::any, sizeof...(Ts)> v;
    return makeArgPropObject_<T, Ts...>(p, v, prop, I{});
}

template <GameModel V>
std::shared_ptr<GameObject> makeObstacleObject(const Point3D& p,
                                               const std::string& prop) {
    coord_t sx = 0, sy = 0, sz = 0;
    if (s_sscanf(prop.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &sx,
                 &sy, &sz) < 3)
        LOG_WARN("invalid angled object prop");
    return std::make_shared<Obstacle>(p, Orient3D(sx, sy, sz), V);
}

using object_maker_t = std::function<std::shared_ptr<GameObject>(
    const Point3D&, const std::string&)>;

static const std::unordered_map<std::string, object_maker_t> nameMap = {
    {"checkpoint", makeStandardObject<CheckpointScript>},
    {"stageend", makeStandardObject<StageEndScript>},
    {"gameend", makeStandardObject<GameEndScript>},
    {"setspeed", makePropObject<SetSpeedScript>},
    {"cavep0", makeObstacleObject<GameModel::CavePillar0>},
    {"cavep1", makeObstacleObject<GameModel::CavePillar1>},
    {"cavep2", makeObstacleObject<GameModel::CavePillar2>},
    {"column0", makeObstacleObject<GameModel::Column0>},
    {"column1", makeObstacleObject<GameModel::Column1>},
    {"column2", makeObstacleObject<GameModel::Column2>},
    {"column3", makeObstacleObject<GameModel::Column3>},
    {"box", makeCoordPropObject<Box, 3>},
    {"sbox", makeCoordPropObject<SlidingBox, 9>},
    {"sboxs", makeCoordPropObject<SlidingBoxSine, 9>},
    {"mbox", makeArgPropObject<MovingBox, int, coord_t, coord_t, coord_t,
                               coord_t, coord_t, coord_t, coord_t, coord_t>},
    {"shard", makeStandardObject<EnemyShard>},
    {"gunboat", makeStandardObject<EnemyGunboat>},
    {"volcano", makeAngleVelObject<EnemyVolcano>},
    {"chevron", makeStandardObject<EnemyChevron>},
    {"fighter", makeArgPropObject<EnemyFighter, int>},
    {"wave", makeCoordPropObject<EnemyWave, 5>},
    {"turret", makeAngleObject<EnemyTurret>},
    {"walker", makeArgPropObject<EnemyWalker, int>},
    {"spider", makeStandardObject<EnemySpider>},
    {"gunboat2", makeStandardObject<EnemyGunboat2>},
    {"wasp", makeStandardObject<EnemyWasp>},
    {"rammer", makeStandardObject<EnemyRammer>},
    {"bouncer", makeTwoPosObject<EnemyBouncer>},
    {"destroyer", makeArgPropObject<EnemyDestroyer, int>},
    {"orbiter", makeArgPropObject<EnemyOrbiter, int>},
    {"wturret", makeAngleObject<EnemyWheeledTurret>},
    {"sturret", makeAngleObject<EnemySpreadTurret>},
    {"launcher", makeAngleObject<EnemyLauncher>},
    {"pod", makeAngleObject<EnemyPod>},
    {"blocker", makeStandardObject<EnemyBlocker>},
    {"pewpew", makeAngleObject<EnemyPewpew>},
    {"boss0", makeStandardObject<EnemyBoss0>},
    {"boss1", makeStandardObject<EnemyBoss1>},
    {"boss2", makeStandardObject<EnemyBoss2>},
    {"boss3", makeStandardObject<EnemyBoss3>},
    {"boss4", makeStandardObject<Boss4Script>},
    //{"boss4", makeStandardObject<EnemyBoss4>},
    {"boss5", makeStandardObject<EnemyBoss5>},
    {"boss6", makeStandardObject<EnemyBoss6>},
    {"boss7", makeStandardObject<EnemyBoss7>},
};

std::shared_ptr<GameObject> loadObjectSpawn(Point3D p, const std::string& name,
                                            const std::string& prop) {
    auto it = nameMap.find(name);
    if (it == nameMap.end()) {
        never("unrecognized object name");
        return nullptr;
    }
    return (*it).second(p, prop);
}

}  // namespace hiemalia
