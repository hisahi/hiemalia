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
    Point3D p;
    coord_t r{0};
    Point3D p1{0, 0, 0};
    Point3D p2{0, 0, 0};

    inline static CollisionShape blank() {
        return CollisionShape(CollisionShapeType::Point, Point3D(0, 0, 0));
    }
    inline static CollisionShape point(const Point3D& m) {
        return CollisionShape(CollisionShapeType::Point, m);
    }
    inline static CollisionShape line(const Point3D& a, const Point3D& b) {
        return CollisionShape(CollisionShapeType::Line, a, b);
    }
    inline static CollisionShape cuboid(const Point3D& a, const Point3D& b) {
        return CollisionShape(CollisionShapeType::Cuboid, a, b);
    }
    inline static CollisionShape sphere(const Point3D& c, coord_t r) {
        return CollisionShape(CollisionShapeType::Sphere, c, r);
    }
    inline static CollisionShape tri(const Point3D& a, const Point3D& b,
                                     const Point3D& c) {
        return CollisionShape(CollisionShapeType::Tri, a, b, c);
    }

  private:
    CollisionShape(CollisionShapeType type, const Point3D& p)
        : type(type), p(p) {}
    CollisionShape(CollisionShapeType type, const Point3D& a, const Point3D& b)
        : type(type), p(a), p1(b) {}
    CollisionShape(CollisionShapeType type, const Point3D& a, const Point3D& b,
                   const Point3D& c)
        : type(type), p(a), p1(b), p2(c) {}
    CollisionShape(CollisionShapeType type, const Point3D& p, coord_t r2)
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

bool collidesPointSphere(const Point3D& point, const Point3D& center,
                         coord_t radius);
bool collidesLineSphere(const Point3D& line1, const Point3D& line2,
                        const Point3D& center, coord_t radius);
bool collidesPointCuboid(const Point3D& point, const Point3D& cuboid1,
                         const Point3D& cuboid2);
bool collidesLineCuboid(const Point3D& line1, const Point3D& line2,
                        const Point3D& cuboid1, const Point3D& cuboid2);
bool collidesCuboidPoint(const Point3D& cuboid1, const Point3D& cuboid2,
                         const Point3D& point);
bool collidesCuboidLine(const Point3D& cuboid1, const Point3D& cuboid2,
                        const Point3D& line1, const Point3D& line2);
bool collidesSpherePoint(const Point3D& center, coord_t radius,
                         const Point3D& point);
bool collidesSphereLine(const Point3D& center, coord_t radius,
                        const Point3D& line1, const Point3D& line2);
bool collidesSphereCuboid(const Point3D& center, coord_t radius,
                          const Point3D& cuboid1, const Point3D& cuboid2);
bool collidesCuboidSphere(const Point3D& cuboid1, const Point3D& cuboid2,
                          const Point3D& center, coord_t radius);
bool collidesCuboidCuboid(const Point3D& cuboid1_1, const Point3D& cuboid1_2,
                          const Point3D& cuboid2_1, const Point3D& cuboid2_2);
bool collidesSphereSphere(const Point3D& center1, coord_t radius1,
                          const Point3D& center2, coord_t radius2);
bool collidesLineTri(const Point3D& line1, const Point3D& line2,
                     const Point3D& tri1, const Point3D& tri2,
                     const Point3D& tri3);
bool collidesCuboidTri(const Point3D& cuboid1, const Point3D& cuboid2,
                       const Point3D& tri1, const Point3D& tri2,
                       const Point3D& tri3);
bool collidesSphereTri(const Point3D& center, coord_t radius,
                       const Point3D& tri1, const Point3D& tri2,
                       const Point3D& tri3);
bool collidesTriTri(const Point3D& tri1_1, const Point3D& tri1_2,
                    const Point3D& tri1_3, const Point3D& tri2_1,
                    const Point3D& tri2_2, const Point3D& tri2_3);

struct CollisionSphere {
    Point3D center;
    coord_t radius;
};

bool collidesLineShape(const Point3D& p1, const Point3D& p2,
                       const CollisionShape& shape, const Matrix3D& mat);
bool collidesCuboidShape(const Point3D& c1, const Point3D& c2,
                         const CollisionShape& shape, const Matrix3D& mat);
bool collidesSphereShape(const Point3D& c, coord_t r,
                         const CollisionShape& shape, const Matrix3D& mat);

bool collidesLineModel(const Point3D& p1, const Point3D& p2,
                       const ModelCollision& mc, const Matrix3D& mat);
bool collidesCuboidModel(const Point3D& c1, const Point3D& c2,
                         const ModelCollision& mc, const Matrix3D& mat);
bool collidesSphereModel(const Point3D& c, coord_t r, const ModelCollision& mc,
                         const Matrix3D& mat);
bool collidesModelModel(const ModelCollision& mc1, const Matrix3D& mat1,
                        const ModelCollision& mc2, const Matrix3D& mat2);

bool collidesSweepSpherePoint(const Point3D& c1, const Point3D& c2, coord_t r,
                              const Point3D& p);
bool collidesSweepSphereModel(const Point3D& c1, const Point3D& c2, coord_t r,
                              const ModelCollision& mc, const Matrix3D& mat);

Point3D collidesSweepSphereSphereWhere(const Point3D& line1,
                                       const Point3D& line2, coord_t radius,
                                       const Point3D& s_center,
                                       coord_t s_radius);
Point3D collidesSweepSphereCuboidWhere(const Point3D& line1,
                                       const Point3D& line2, coord_t radius,
                                       const Point3D& cuboid1,
                                       const Point3D& cuboid2);
Point3D collidesSweepSphereModelWhere(const Point3D& line1,
                                      const Point3D& line2, coord_t radius,
                                      const ModelCollision& mc,
                                      const Matrix3D& mat);
Point3D collidesCuboidPointDirection(const Point3D& them, const Point3D& me,
                                     const Point3D& mySize);

};  // namespace hiemalia

#endif  // M_COLLIDE_HH
