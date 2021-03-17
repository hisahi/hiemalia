/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// namepair.hh: header file for NamePair

#ifndef M_NAMEPAIR_HH
#define M_NAMEPAIR_HH

#include <string>

namespace hiemalia {
template <typename T>
class NamePair {
   public:
    NamePair(const std::string& name, T value) noexcept
        : name(name), value(value) {}
    const std::string name;
    const T value;
};
};  // namespace hiemalia

#endif  // M_NAMEPAIR_HH
