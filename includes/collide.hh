/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// collide.hh: header file for the collision engine

#ifndef M_COLLIDE_HH
#define M_COLLIDE_HH

#include "defs.hh"
#include "model.hh"

namespace hiemalia {
bool collidesPointSphere(const ModelPoint& point, const ModelPoint& center,
                         coord_t radius2);
bool collidesLineSphere(const ModelPoint& line1, const ModelPoint& line2,
                        const ModelPoint& center, coord_t radius2);
bool collidesPointCuboid(const ModelPoint& point, const ModelPoint& cuboid1,
                         const ModelPoint& cuboid2);
bool collidesLineCuboid(const ModelPoint& line1, const ModelPoint& line2,
                        const ModelPoint& cuboid1, const ModelPoint& cuboid2);
bool collidesSphereCuboid(const ModelPoint& center, coord_t radius2,
                          const ModelPoint& cuboid1, const ModelPoint& cuboid2);
bool collidesCuboidSphere(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                          const ModelPoint& center, coord_t radius2);
bool collidesCuboidCuboid(const ModelPoint& cuboid1_1,
                          const ModelPoint& cuboid1_2,
                          const ModelPoint& cuboid2_1,
                          const ModelPoint& cuboid2_2);
bool collidesSphereSphere(const ModelPoint& center1, coord_t radius1_2,
                          const ModelPoint& center2, coord_t radius2_2);
bool collidesLineTri(const ModelPoint& line1, const ModelPoint& line2,
                     const ModelPoint& tri1, const ModelPoint& tri2,
                     const ModelPoint& tri3);
bool collidesSphereTri(const ModelPoint& center, coord_t radius2,
                       const ModelPoint& tri1, const ModelPoint& tri2,
                       const ModelPoint& tri3);

struct CollisionSphere {
    ModelPoint center;
    coord_t radius2;
};
};  // namespace hiemalia

#endif  // M_COLLIDE_HH
