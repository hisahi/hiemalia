/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// load3d.hh: header file for load3d.cc

#ifndef M_LOAD3D_HH
#define M_LOAD3D_HH

#include <vector>

#include "collide.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
struct ModelWithCollision {
    Model model;
    ModelCollision collision;
    coord_t radius;
};

Model load3D(const std::string& filename);
Model load3D(const std::string& folder, const std::string& filename);

ModelWithCollision load3DWithCollision(const std::string& filename);
ModelWithCollision load3DWithCollision(const std::string& folder,
                                       const std::string& filename);

};  // namespace hiemalia

#endif  // M_LOAD3D_HH
