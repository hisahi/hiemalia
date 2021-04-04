/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// collide.cc: implementation of collision algorithms

#include "collide.hh"

#include <algorithm>
#include <cmath>
#include <tuple>

#include "logger.hh"
#include "math.hh"

namespace hiemalia {
static inline coord_t distanceSquared(const ModelPoint& p1,
                                      const ModelPoint& p2) {
    return (p1 - p2).lengthSquared();
}

static inline coord_t distanceEuclidean(const ModelPoint& p1,
                                        const ModelPoint& p2) {
    return (p1 - p2).length();
}

static inline coord_t pointDot(const ModelPoint& p1, const ModelPoint& p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

static inline ModelPoint pointCross(const ModelPoint& p1,
                                    const ModelPoint& p2) {
    return ModelPoint(p1.y * p2.z - p1.z * p1.y, p1.x * p2.z - p1.z * p1.x,
                      p1.x * p2.y - p1.y * p1.x);
}

static bool collidesPointRange(coord_t x, coord_t y1, coord_t y2) {
    if (y1 > y2) std::swap(y1, y2);
    return y1 <= x && x <= y2;
}

static bool collidesRangeRange(coord_t x1, coord_t x2, coord_t y1, coord_t y2) {
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    return x1 <= y2 && y1 <= x2;
}

static ModelPoint closestPointOnLineToPoint(const ModelPoint& l1,
                                            const ModelPoint& l2,
                                            const ModelPoint& p) {
    ModelPoint p1 = l2 - l1;
    ModelPoint p2 = p - l1;
    if (pointDot(p2, p1) <= 0) return l1;

    ModelPoint p3 = p - l2;
    if (pointDot(p3, p1) >= 0) return l2;

    coord_t t = pointDot(p2.normalize(), p1.normalize());
    return lerpPoint(l1, t, l2);
}

static ModelPoint ontoLineSegment(const ModelPoint& l1, const ModelPoint& x,
                                  const ModelPoint& l2) {
    return lerpPoint(
        l1,
        clamp<coord_t>(0, pointDot((l2 - l1).normalize(), (x - l1).normalize()),
                       1),
        l2);
}

static ModelPoint closestPointOnLineToLine(const ModelPoint& t1,
                                           const ModelPoint& t2,
                                           const ModelPoint& o1,
                                           const ModelPoint& o2) {
    ModelPoint a = o2 - o1;
    ModelPoint b = t1 - o1;
    ModelPoint c = t2 - o1;
    b -= pointDot(b, a) * a;
    c -= pointDot(c, a) * a;
    ModelPoint d = c - b;
    coord_t t =
        (d - c).lengthSquared() < 1e-9 ? 0 : pointDot(-b, d) / pointDot(d, d);
    ModelPoint l = lerpPoint(t1, clamp<coord_t>(0, t, 1), t2);
    return ontoLineSegment(t1, ontoLineSegment(o1, l, o2), t2);
}

static coord_t closestDistanceToLineFromPoint(const ModelPoint& l1,
                                              const ModelPoint& l2,
                                              const ModelPoint& p) {
    return (closestPointOnLineToPoint(l1, l2, p) - p).length();
}

static coord_t closestDistanceBetweenTwoLines(const ModelPoint& a1,
                                              const ModelPoint& a2,
                                              const ModelPoint& b1,
                                              const ModelPoint& b2) {
    return (closestPointOnLineToLine(a1, a2, b1, b2) -
            closestPointOnLineToLine(b1, b2, a1, a2))
        .length();
}

std::tuple<coord_t, coord_t> pointFind2D(ModelPoint tx, ModelPoint ty,
                                         ModelPoint tz, ModelPoint p) {
    // compute partial inverse matrix
    coord_t fac = 1 / (tx.x * (ty.y * tz.z - tz.y * ty.z) +
                       tx.y * (tx.y * tz.z - tz.y * tx.z) +
                       tx.z * (tx.y * ty.z - ty.y * tx.z));
    coord_t ixx = fac * (ty.y * tz.z - tz.y * ty.z);
    coord_t iyx = fac * (ty.z * tz.x - ty.x * tz.z);
    coord_t izx = fac * (ty.x * tz.y - tz.x * ty.y);
    coord_t ixy = fac * (tx.z * tz.y - tx.y * tz.z);
    coord_t iyy = fac * (tx.x * tz.z - tx.z * tz.x);
    coord_t izy = fac * (tz.x * tx.y - tx.x * tz.y);
    return std::make_tuple(p.x * ixx + p.y * iyx + p.z * izx,
                           p.y * ixy + p.y * iyy + p.z * izy);
}

static coord_t closestDistanceToTriFromPoint(const ModelPoint& t1,
                                             const ModelPoint& t2,
                                             const ModelPoint& t3,
                                             const ModelPoint& p0) {
    ModelPoint x = t2 - t1;
    ModelPoint y = t3 - t1;
    ModelPoint p = p0 - t1;
    ModelPoint n = pointCross(x, y).normalize();
    coord_t dz = pointDot(p, n);
    ModelPoint q = p - n * dz;
    auto [tx, ty] = pointFind2D(x, y, n, q);
    coord_t d, dx, dy;
    if (tx >= 0 || ty >= 0) {
        if (ty < tx - 1) {
            tx -= 1;
            d = sqrt(tx * tx + ty * ty);
        } else if (tx < ty - 1) {
            ty -= 1;
            d = sqrt(tx * tx + ty * ty);
        } else {
            d = (tx + ty - 1) * numbers::ISQRT2<coord_t>;
        }
    } else {
        if (tx < 0)
            dx = -tx;
        else if (tx > 1)
            dx = tx - 1;
        else
            dx = 0;
        if (ty < 0)
            dy = -ty;
        else if (ty > 1)
            dy = ty - 1;
        else
            dy = 0;
        d = sqrt(dx * dx + dy * dy);
    }
    if (d < 0) d = 0;
    return sqrt(d * d + dz * dz);
}

bool collidesPointSphere(const ModelPoint& p, const ModelPoint& c, coord_t r) {
    return distanceEuclidean(p, c) <= r;
}

bool collidesLineSphere(const ModelPoint& l1, const ModelPoint& l2,
                        const ModelPoint& c, coord_t r) {
    return closestDistanceToLineFromPoint(l1, l2, c) <= r;
}

bool collidesPointCuboid(const ModelPoint& p, const ModelPoint& c1,
                         const ModelPoint& c2) {
    return collidesPointRange(p.x, c1.x, c2.x) &&
           collidesPointRange(p.y, c1.y, c2.y) &&
           collidesPointRange(p.z, c1.z, c2.z);
}

bool collidesLineCuboid(const ModelPoint& l1, const ModelPoint& l2,
                        const ModelPoint& c1, const ModelPoint& c2) {
    if (collidesPointCuboid(l1, c1, c2) && collidesPointCuboid(l2, c1, c2))
        return true;

    coord_t lx1 = unlerp(c1.x, l1.x, c2.x);
    coord_t lx2 = unlerp(c1.x, l2.x, c2.x);
    coord_t ly1 = unlerp(c1.y, l1.y, c2.y);
    coord_t ly2 = unlerp(c1.y, l2.y, c2.y);
    coord_t lz1 = unlerp(c1.z, l1.z, c2.z);
    coord_t lz2 = unlerp(c1.z, l2.z, c2.z);

    return (collidesPointRange(lx1, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, lx1, l2), c1, c2)) ||
           (collidesPointRange(lx2, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, lx2, l2), c1, c2)) ||
           (collidesPointRange(ly1, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, ly1, l2), c1, c2)) ||
           (collidesPointRange(ly2, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, ly2, l2), c1, c2)) ||
           (collidesPointRange(lz1, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, lz1, l2), c1, c2)) ||
           (collidesPointRange(lz2, 0, 1) &&
            collidesPointCuboid(lerpPoint(l1, lz2, l2), c1, c2));
}

bool collidesSphereCuboid(const ModelPoint& c, coord_t r, const ModelPoint& c1,
                          const ModelPoint& c2) {
    coord_t r2 = r * r;
    coord_t x1 = std::min(c1.x, c2.x), x2 = std::max(c1.x, c2.x);
    coord_t y1 = std::min(c1.y, c2.y), y2 = std::max(c1.y, c2.y);
    coord_t z1 = std::min(c1.z, c2.z), z2 = std::max(c1.z, c2.z);

    if (c.x < x1) r2 -= (c.x - x1) * (c.x - x1);
    if (c.x > x2) r2 -= (c.x - x2) * (c.x - x2);
    if (c.y < y1) r2 -= (c.y - y1) * (c.y - y1);
    if (c.y > y2) r2 -= (c.y - y2) * (c.y - y2);
    if (c.z < z1) r2 -= (c.z - z1) * (c.z - z1);
    if (c.z > z2) r2 -= (c.z - z2) * (c.z - z2);

    return r2 >= 0;
}

bool collidesCuboidCuboid(const ModelPoint& a1, const ModelPoint& a2,
                          const ModelPoint& b1, const ModelPoint& b2) {
    return collidesRangeRange(a1.x, a2.x, b1.x, b2.x) &&
           collidesRangeRange(a1.y, a2.y, b1.y, b2.y) &&
           collidesRangeRange(a1.z, a2.z, b1.z, b2.z);
}

bool collidesSphereSphere(const ModelPoint& c1, coord_t r1,
                          const ModelPoint& c2, coord_t r2) {
    return distanceEuclidean(c1, c2) <= r1 + r2;
}

bool collidesLineTri(const ModelPoint& l1, const ModelPoint& l2,
                     const ModelPoint& t1, const ModelPoint& t2,
                     const ModelPoint& t3) {
    static const coord_t sensitivity = 1.0 / 1024;
    ModelPoint a = l1 - t1;
    ModelPoint b = l2 - t1;
    ModelPoint x = t2 - t1;
    ModelPoint y = t3 - t1;
    ModelPoint z = pointCross(x, y).normalize();

    coord_t c1 = pointDot(a, z) - sensitivity;
    coord_t c2 = pointDot(b, z) + sensitivity;
    if (c1 * c2 > 0) return false;

    ModelPoint p = lerpPoint(a, unlerp<coord_t>(c1, 0, c2), b);
    auto [tx, ty] = pointFind2D(x, y, z, p);
    return 0 <= tx && tx <= 1 && 0 <= ty && ty <= 1 && tx + ty <= 1;
}

bool collidesCuboidTri(const ModelPoint& c1, const ModelPoint& c2,
                       const ModelPoint& t1, const ModelPoint& t2,
                       const ModelPoint& t3) {
    return collidesLineCuboid(t1, t2, c1, c2) &&
           collidesLineCuboid(t1, t3, c1, c2) &&
           collidesLineCuboid(t2, t3, c1, c2);
}

bool collidesSphereTri(const ModelPoint& c, coord_t r, const ModelPoint& t1,
                       const ModelPoint& t2, const ModelPoint& t3) {
    return closestDistanceToTriFromPoint(t1, t2, t3, c) <= r;
}

bool collidesCuboidSphere(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                          const ModelPoint& center, coord_t radius2) {
    return collidesSphereCuboid(center, radius2, cuboid1, cuboid2);
}

bool collidesCuboidPoint(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                         const ModelPoint& point) {
    return collidesPointCuboid(point, cuboid1, cuboid2);
}

bool collidesCuboidLine(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                        const ModelPoint& line1, const ModelPoint& line2) {
    return collidesLineCuboid(line1, line2, cuboid1, cuboid2);
}

bool collidesSpherePoint(const ModelPoint& center, coord_t radius,
                         const ModelPoint& point) {
    return collidesPointSphere(point, center, radius);
}

bool collidesSphereLine(const ModelPoint& center, coord_t radius,
                        const ModelPoint& line1, const ModelPoint& line2) {
    return collidesLineSphere(line1, line2, center, radius);
}

bool collidesTriTri(const ModelPoint& ta1, const ModelPoint& ta2,
                    const ModelPoint& ta3, const ModelPoint& tb1,
                    const ModelPoint& tb2, const ModelPoint& tb3) {
    return collidesLineTri(ta1, ta2, tb1, tb2, tb3) ||
           collidesLineTri(ta2, ta3, tb1, tb2, tb3) ||
           collidesLineTri(ta3, ta1, tb1, tb2, tb3);
}

bool collidesSweepSpherePoint(const ModelPoint& c1, const ModelPoint& c2,
                              coord_t r, const ModelPoint& p) {
    return closestDistanceToLineFromPoint(c1, c2, p) < r;
}

static bool collidesSweepSphereLine(const ModelPoint& c1, const ModelPoint& c2,
                                    coord_t r, const ModelPoint& l1,
                                    const ModelPoint& l2) {
    return closestDistanceBetweenTwoLines(c1, c2, l1, l2) < r;
}

static bool collidesSweepSphereCuboid(const ModelPoint& c1,
                                      const ModelPoint& c2, coord_t r,
                                      const ModelPoint& q1,
                                      const ModelPoint& q2) {
    if (std::min(c1.x, c2.x) <= q1.x && std::max(c1.x, c2.x) >= q1.x &&
        collidesSphereCuboid(
            lerpPoint(c1, unlerp(c1.x, (q1.x + q2.x) / 2, c2.x), c2), r, q1,
            q2))
        return true;
    if (std::min(c1.y, c2.y) <= q1.y && std::max(c1.y, c2.y) >= q1.y &&
        collidesSphereCuboid(
            lerpPoint(c1, unlerp(c1.y, (q1.y + q2.y) / 2, c2.y), c2), r, q1,
            q2))
        return true;
    if (std::min(c1.z, c2.z) <= q1.z && std::max(c1.z, c2.z) >= q1.z &&
        collidesSphereCuboid(
            lerpPoint(c1, unlerp(c1.z, (q1.z + q2.z) / 2, c2.z), c2), r, q1,
            q2))
        return true;
    return collidesSphereCuboid(c1, r, q1, q2) ||
           collidesSphereCuboid(c2, r, q1, q2);
}

static bool collidesSweepSphereSphere(const ModelPoint& c1,
                                      const ModelPoint& c2, coord_t r,
                                      const ModelPoint& sc, coord_t sr) {
    return closestDistanceToLineFromPoint(c1, c2, sc) < r + sr;
}

static bool collidesSweepSphereTri(const ModelPoint& c1, const ModelPoint& c2,
                                   coord_t r, const ModelPoint& t1,
                                   const ModelPoint& t2, const ModelPoint& t3) {
    ModelPoint tx = (t1 + t2 + t3) * (1.0 / 3);
    ModelPoint cx = ModelPoint::average(c1, c2);
    ModelPoint cd = c2 - c1;
    ModelPoint td = tx - cx;
    td -= cd * pointDot(td, cd);
    if (td.length() > r) td = td.normalize() * r;
    return collidesSphereTri(c1, r, t1, t2, t3) ||
           collidesSphereTri(c2, r, t1, t2, t3) ||
           collidesLineTri(c1 + td, c2 + td, t1, t2, t3);
}

static ModelPoint collidesSweepSpherePointWhere(const ModelPoint& l1,
                                                const ModelPoint& l2, coord_t r,
                                                const ModelPoint& p) {
    return p;
}

static ModelPoint collidesSweepSphereLineWhere(const ModelPoint& l1,
                                               const ModelPoint& l2, coord_t r,
                                               const ModelPoint& a1,
                                               const ModelPoint& a2) {
    return closestPointOnLineToLine(l1, l2, a1, a2);
}

ModelPoint collidesSweepSphereSphereWhere(const ModelPoint& l1,
                                          const ModelPoint& l2, coord_t r,
                                          const ModelPoint& sc, coord_t sr) {
    coord_t a = pointDot(l2, l2);
    coord_t b = 2 * pointDot(l2, l1 - sc);
    coord_t c = pointDot(l1 - sc, l1 - sc) - sr * sr;
    coord_t d = b * b - 4 * a * c;
    if (d < 0) {
        LOG_WARN("tried to find intersection point, but there is none");
        return l1;
    }
    coord_t q = 0.5 / a;
    d = sqrt(d);
    return lerpPoint(l1, std::min(q * (-b + d), q * (-b - d)), l2);
}

static coord_t validateCuboidUnlerp(coord_t x) { return x < 0 ? 1 : x; }

ModelPoint collidesSweepSphereCuboidWhere(const ModelPoint& l1,
                                          const ModelPoint& l2, coord_t r,
                                          const ModelPoint& c1,
                                          const ModelPoint& c2) {
    coord_t tx = validateCuboidUnlerp(l2.x > l1.x ? unlerp(l1.x, c1.x, l2.x)
                                                  : unlerp(l1.x, c2.x, l2.x));
    coord_t ty = validateCuboidUnlerp(l2.y > l1.y ? unlerp(l1.y, c1.y, l2.y)
                                                  : unlerp(l1.y, c2.y, l2.y));
    coord_t tz = validateCuboidUnlerp(l2.z > l1.z ? unlerp(l1.z, c1.z, l2.z)
                                                  : unlerp(l1.z, c2.z, l2.z));
    return lerpPoint(l1, std::min({tx, ty, tz}), l2);
}

static ModelPoint collidesSweepSphereTriWhere(const ModelPoint& c1,
                                              const ModelPoint& c2, coord_t r,
                                              const ModelPoint& t1,
                                              const ModelPoint& t2,
                                              const ModelPoint& t3) {
    ModelPoint tx = (t1 + t2 + t3) * (1.0 / 3);
    ModelPoint cx = ModelPoint::average(c1, c2);
    ModelPoint cd = c2 - c1;
    ModelPoint td = tx - cx;
    td -= cd * pointDot(td, cd);
    if (td.length() > r) td = td.normalize() * r;
    if (collidesSphereTri(c1, r, t1, t2, t3)) return c1;
    if (collidesSphereTri(c2, r, t1, t2, t3)) return c2;

    static const coord_t sensitivity = 1.0 / 1024;
    ModelPoint a = c1 - t1;
    ModelPoint b = c2 - t1;
    ModelPoint x = t2 - t1;
    ModelPoint y = t3 - t1;
    ModelPoint z = pointCross(x, y).normalize();

    coord_t x1 = pointDot(a, z) - sensitivity;
    coord_t x2 = pointDot(b, z) + sensitivity;
    return lerpPoint(a, unlerp<coord_t>(x1, 0, x2), b);
}

bool collidesPointShape(const ModelPoint& p, const CollisionShape& shape,
                        const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
        case CollisionShapeType::Line:
        case CollisionShapeType::Tri:
            return false;
        case CollisionShapeType::Cuboid:
            return collidesPointCuboid(p, mat.project(shape.p),
                                       mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesPointSphere(p, mat.project(shape.p), shape.r);
        default:
            never("invalid shape");
    }
}

bool collidesLineShape(const ModelPoint& p1, const ModelPoint& p2,
                       const CollisionShape& shape, const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
        case CollisionShapeType::Line:
            return false;
        case CollisionShapeType::Cuboid:
            return collidesLineCuboid(p1, p2, mat.project(shape.p),
                                      mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesLineSphere(p1, p2, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesLineTri(p1, p2, mat.project(shape.p),
                                   mat.project(shape.p1),
                                   mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

bool collidesCuboidShape(const ModelPoint& c1, const ModelPoint& c2,
                         const CollisionShape& shape, const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesCuboidPoint(c1, c2, mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesCuboidLine(c1, c2, mat.project(shape.p),
                                      mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesCuboidCuboid(c1, c2, mat.project(shape.p),
                                        mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesCuboidSphere(c1, c2, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesCuboidTri(c1, c2, mat.project(shape.p),
                                     mat.project(shape.p1),
                                     mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

bool collidesSphereShape(const ModelPoint& c, coord_t r2,
                         const CollisionShape& shape, const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSpherePoint(c, r2, mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSphereLine(c, r2, mat.project(shape.p),
                                      mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSphereCuboid(c, r2, mat.project(shape.p),
                                        mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSphereSphere(c, r2, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesSphereTri(c, r2, mat.project(shape.p),
                                     mat.project(shape.p1),
                                     mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

static bool collidesTriShape(const ModelPoint& t0, const ModelPoint& t1,
                             const ModelPoint& t2, const CollisionShape& shape,
                             const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return false;
        case CollisionShapeType::Line:
            return collidesLineTri(mat.project(shape.p), mat.project(shape.p1),
                                   t0, t1, t2);
        case CollisionShapeType::Cuboid:
            return collidesCuboidTri(mat.project(shape.p),
                                     mat.project(shape.p1), t0, t1, t2);
        case CollisionShapeType::Sphere:
            return collidesSphereTri(mat.project(shape.p), shape.r, t0, t1, t2);
        case CollisionShapeType::Tri:
            return collidesTriTri(mat.project(shape.p), mat.project(shape.p1),
                                  mat.project(shape.p2), t0, t1, t2);
        default:
            never("invalid shape");
    }
}

bool collidesPointModel(const ModelPoint& p, const ModelCollision& mc,
                        const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesPointShape(p, shape, mat);
                       });
}

bool collidesLineModel(const ModelPoint& p1, const ModelPoint& p2,
                       const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesLineShape(p1, p2, shape, mat);
                       });
}

bool collidesCuboidModel(const ModelPoint& c1, const ModelPoint& c2,
                         const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesCuboidShape(c1, c2, shape, mat);
                       });
}

bool collidesSphereModel(const ModelPoint& c, coord_t r2,
                         const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesSphereShape(c, r2, shape, mat);
                       });
}

static bool collidesTriModel(const ModelPoint& t0, const ModelPoint& t1,
                             const ModelPoint& t2, const ModelCollision& mc,
                             const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesTriShape(t0, t1, t2, shape, mat);
                       });
}

static bool collidesShapeModel(const CollisionShape& shape,
                               const Matrix3D& mat1, const ModelCollision& mc,
                               const Matrix3D& mat2) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesPointModel(mat1.project(shape.p), mc, mat2);
        case CollisionShapeType::Line:
            return collidesLineModel(mat1.project(shape.p),
                                     mat1.project(shape.p1), mc, mat2);
        case CollisionShapeType::Cuboid:
            return collidesCuboidModel(mat1.project(shape.p),
                                       mat1.project(shape.p1), mc, mat2);
        case CollisionShapeType::Sphere:
            return collidesSphereModel(mat1.project(shape.p), shape.r, mc,
                                       mat2);
        case CollisionShapeType::Tri:
            return collidesTriModel(mat1.project(shape.p),
                                    mat1.project(shape.p1),
                                    mat1.project(shape.p2), mc, mat2);
        default:
            never("invalid shape");
    }
}

bool collidesModelModel(const ModelCollision& mc1, const Matrix3D& mat1,
                        const ModelCollision& mc2, const Matrix3D& mat2) {
    return std::any_of(mc1.shapes.begin(), mc1.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesShapeModel(shape, mat1, mc2, mat2);
                       });
}

static bool collidesSweepSphereShape(const ModelPoint& c1, const ModelPoint& c2,
                                     coord_t r2, const CollisionShape& shape,
                                     const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSweepSpherePoint(c1, c2, r2, mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSweepSphereLine(c1, c2, r2, mat.project(shape.p),
                                           mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSweepSphereCuboid(c1, c2, r2, mat.project(shape.p),
                                             mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSweepSphereSphere(c1, c2, r2, mat.project(shape.p),
                                             shape.r);
        case CollisionShapeType::Tri:
            return collidesSweepSphereTri(c1, c2, r2, mat.project(shape.p),
                                          mat.project(shape.p1),
                                          mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

static ModelPoint collidesSweepSphereShapeWhere(const ModelPoint& c1,
                                                const ModelPoint& c2,
                                                coord_t r2,
                                                const CollisionShape& shape,
                                                const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSweepSpherePointWhere(c1, c2, r2,
                                                 mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSweepSphereLineWhere(
                c1, c2, r2, mat.project(shape.p), mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSweepSphereCuboidWhere(
                c1, c2, r2, mat.project(shape.p), mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSweepSphereSphereWhere(
                c1, c2, r2, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesSweepSphereTriWhere(c1, c2, r2, mat.project(shape.p),
                                               mat.project(shape.p1),
                                               mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

bool collidesSweepSphereModel(const ModelPoint& c1, const ModelPoint& c2,
                              coord_t r, const ModelCollision& mc,
                              const Matrix3D& mat) {
    return std::any_of(
        mc.shapes.begin(), mc.shapes.end(), [&](const CollisionShape& shape) {
            return collidesSweepSphereShape(c1, c2, r, shape, mat);
        });
}

ModelPoint collidesSweepSphereModelWhere(const ModelPoint& l1,
                                         const ModelPoint& l2, coord_t r,
                                         const ModelCollision& mc,
                                         const Matrix3D& mat) {
    for (const CollisionShape& shape : mc.shapes) {
        if (collidesSweepSphereShape(l1, l2, r, shape, mat))
            return collidesSweepSphereShapeWhere(l1, l2, r, shape, mat);
    }
    return l1;
}

}  // namespace hiemalia
