/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// load2d.hh: header file for load2d.cc

#ifndef M_LOAD2D_HH
#define M_LOAD2D_HH

#include <vector>

#include "font.hh"
#include "shape.hh"

namespace hiemalia {
Color parseColor(std::string s);
ShapeSheet load2D(const std::string& filename);
Font loadFont(const std::string& filename);

};  // namespace hiemalia

#endif  // M_LOAD2D_HH
