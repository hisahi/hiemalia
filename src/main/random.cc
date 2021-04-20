/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// random.cc: implementation of randomization

#include "random.hh"

#include "logger.hh"

namespace hiemalia {

static std::random_device rd;
static random_engine re{rd()};

random_engine& getRandomEngine() { return re; }

RandomPool::RandomPool(int idx)
    : engine_(random_pool_engine(324331031U + 766710257U * idx)) {}

static RandomPool pool{0};

RandomPool& getRandomPool() { return pool; }

void restartRandomPool(int idx) {
    LOG_DEBUG("restarting RNG pool - seed %i", idx);
    pool = RandomPool{idx};
}

Point3D randomUnitVector() {
    std::normal_distribution<coord_t> n;
    int i = 0;
    coord_t x, y, z;
    do {
        x = random(n);
        y = random(n);
        z = random(n);
    } while (x == 0 && y == 0 && z == 0 && i++ < 64);

    return i < 64 ? Point3D(x, y, z).normalize() : Point3D(x, y, z);
}

}  // namespace hiemalia
