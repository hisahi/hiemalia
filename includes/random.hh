/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// random.hh: header file for randomization

#ifndef M_RANDOM_HH
#define M_RANDOM_HH

#include <random>

#include "model.hh"

namespace hiemalia {

using random_engine = std::default_random_engine;

random_engine& getRandomEngine();

template <typename T>
auto random(T distr) -> typename T::result_type {
    return distr(getRandomEngine());
}

ModelPoint randomUnitVector();
}  // namespace hiemalia

#endif  // M_RANDOM_HH
