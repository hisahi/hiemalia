/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// models.hh: header file for model file name list

#ifndef M_MODELS_HH
#define M_MODELS_HH

#include <array>
#include <string>

#include "array.hh"

namespace hiemalia {
inline auto modelFileNames = hiemalia::to_array<std::string>({
    "ship.3d",
    // more
});
};  // namespace hiemalia

#endif  // M_MODELS_HH
