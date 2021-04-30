/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// symbol.hh: header file for "symbol" type

#ifndef M_SYMBOL_HH
#define M_SYMBOL_HH

#include <cstdint>

#include "config.hh"
#include "hbase.hh"
#include "inherit.hh"
#include "module.hh"
#include "sbuf.hh"

namespace hiemalia {

using symbol_t = uint32_t;
constexpr symbol_t symbol_none = UINT32_MAX;
constexpr symbol_t index_to_symbol(int i) { return i; }
constexpr symbol_t index_to_symbol(size_t i) {
    return static_cast<symbol_t>(i);
}
constexpr int symbol_to_index(symbol_t i) { return i; }

};  // namespace hiemalia

#endif  // M_SYMBOL_HH
