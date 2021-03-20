/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// load2d.hh: header file for 2D loader

#ifndef M_LOAD2D_HH
#define M_LOAD2D_HH

#include <vector>

#include "rendtext.hh"
#include "shape.hh"

namespace hiemalia {
ShapeSheet load2D(const std::string& filename);
RendererTextFont loadFont(const std::string& filename);

};  // namespace hiemalia

#endif  // M_LOAD2D_HH
