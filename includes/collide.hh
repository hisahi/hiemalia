/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// collide.hh: header file for the collision engine

#ifndef M_COLLIDE_HH
#define M_COLLIDE_HH

#include <algorithm>

#include "defs.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
enum class CollisionShapeType { Point, Line, Cuboid, Sphere, Tri };
struct CollisionShape {
    CollisionShapeType type;
    ModelPoint p;
    coord_t r{0};
    ModelPoint p1{0, 0, 0};
    ModelPoint p2{0, 0, 0};

    inline static CollisionShape blank() {
        return CollisionShape(CollisionShapeType::Point, ModelPoint(0, 0, 0));
    }
    inline static CollisionShape point(const ModelPoint& m) {
        return CollisionShape(CollisionShapeType::Point, m);
    }
    inline static CollisionShape line(const ModelPoint& a,
                                      const ModelPoint& b) {
        return CollisionShape(CollisionShapeType::Line, a, b);
    }
    inline static CollisionShape cuboid(const ModelPoint& a,
                                        const ModelPoint& b) {
        return CollisionShape(CollisionShapeType::Cuboid, a, b);
    }
    inline static CollisionShape sphere(const ModelPoint& c, coord_t r) {
        return CollisionShape(CollisionShapeType::Sphere, c, r);
    }
    inline static CollisionShape tri(const ModelPoint& a, const ModelPoint& b,
                                     const ModelPoint& c) {
        return CollisionShape(CollisionShapeType::Tri, a, b, c);
    }

   private:
    CollisionShape(CollisionShapeType type, const ModelPoint& p)
        : type(type), p(p) {}
    CollisionShape(CollisionShapeType type, const ModelPoint& a,
                   const ModelPoint& b)
        : type(type), p(a), p1(b) {}
    CollisionShape(CollisionShapeType type, const ModelPoint& a,
                   const ModelPoint& b, const ModelPoint& c)
        : type(type), p(a), p1(b), p2(c) {}
    CollisionShape(CollisionShapeType type, const ModelPoint& p, coord_t r2)
        : type(type), p(p), r(r2) {}
};
struct ModelCollision {
    std::vector<CollisionShape> shapes;
    inline ModelCollision() : shapes() {}
    inline ModelCollision(int size) : shapes(size, CollisionShape::blank()) {}
    inline ModelCollision(std::vector<CollisionShape>&& movevec)
        : shapes(std::move(movevec)) {}
};
struct ModelCollisionRadius {
    coord_t hitRadius;
    std::vector<CollisionShape> shapes;
};

bool collidesPointSphere(const ModelPoint& point, const ModelPoint& center,
                         coord_t radius);
bool collidesLineSphere(const ModelPoint& line1, const ModelPoint& line2,
                        const ModelPoint& center, coord_t radius);
bool collidesPointCuboid(const ModelPoint& point, const ModelPoint& cuboid1,
                         const ModelPoint& cuboid2);
bool collidesLineCuboid(const ModelPoint& line1, const ModelPoint& line2,
                        const ModelPoint& cuboid1, const ModelPoint& cuboid2);
bool collidesCuboidPoint(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                         const ModelPoint& point);
bool collidesCuboidLine(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                        const ModelPoint& line1, const ModelPoint& line2);
bool collidesSpherePoint(const ModelPoint& center, coord_t radius,
                         const ModelPoint& point);
bool collidesSphereLine(const ModelPoint& center, coord_t radius,
                        const ModelPoint& line1, const ModelPoint& line2);
bool collidesSphereCuboid(const ModelPoint& center, coord_t radius,
                          const ModelPoint& cuboid1, const ModelPoint& cuboid2);
bool collidesCuboidSphere(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                          const ModelPoint& center, coord_t radius);
bool collidesCuboidCuboid(const ModelPoint& cuboid1_1,
                          const ModelPoint& cuboid1_2,
                          const ModelPoint& cuboid2_1,
                          const ModelPoint& cuboid2_2);
bool collidesSphereSphere(const ModelPoint& center1, coord_t radius1,
                          const ModelPoint& center2, coord_t radius2);
bool collidesLineTri(const ModelPoint& line1, const ModelPoint& line2,
                     const ModelPoint& tri1, const ModelPoint& tri2,
                     const ModelPoint& tri3);
bool collidesCuboidTri(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                       const ModelPoint& tri1, const ModelPoint& tri2,
                       const ModelPoint& tri3);
bool collidesSphereTri(const ModelPoint& center, coord_t radius,
                       const ModelPoint& tri1, const ModelPoint& tri2,
                       const ModelPoint& tri3);
bool collidesTriTri(const ModelPoint& tri1_1, const ModelPoint& tri1_2,
                    const ModelPoint& tri1_3, const ModelPoint& tri2_1,
                    const ModelPoint& tri2_2, const ModelPoint& tri2_3);

struct CollisionSphere {
    ModelPoint center;
    coord_t radius;
};

ModelPoint collidesLineSphereWhere(const ModelPoint& line1,
                                   const ModelPoint& line2,
                                   const ModelPoint& center, coord_t radius);
ModelPoint collidesLineCuboidWhere(const ModelPoint& line1,
                                   const ModelPoint& line2,
                                   const ModelPoint& cuboid1,
                                   const ModelPoint& cuboid2);

bool collidesLineShape(const ModelPoint& p1, const ModelPoint& p2,
                       const CollisionShape& shape, const Matrix3D& mat);
bool collidesCuboidShape(const ModelPoint& c1, const ModelPoint& c2,
                         const CollisionShape& shape, const Matrix3D& mat);
bool collidesSphereShape(const ModelPoint& c, coord_t r2,
                         const CollisionShape& shape, const Matrix3D& mat);

bool collidesLineModel(const ModelPoint& p1, const ModelPoint& p2,
                       const ModelCollision& mc, const Matrix3D& mat);
bool collidesCuboidModel(const ModelPoint& c1, const ModelPoint& c2,
                         const ModelCollision& mc, const Matrix3D& mat);
bool collidesSphereModel(const ModelPoint& c, coord_t r2,
                         const ModelCollision& mc, const Matrix3D& mat);
bool collidesModelModel(const ModelCollision& mc1, const Matrix3D& mat1,
                        const ModelCollision& mc2, const Matrix3D& mat2);

};  // namespace hiemalia

#endif  // M_COLLIDE_HH
