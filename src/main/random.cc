/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// random.cc: implementation of randomization

#include "random.hh"

namespace hiemalia {

static std::random_device rd;
static random_engine re{rd()};

random_engine& getRandomEngine() { return re; }

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
