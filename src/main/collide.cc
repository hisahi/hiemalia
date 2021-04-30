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
// sensitivity
static const coord_t pointRadius = 1.0 / 1024;
/*
static inline coord_t distanceSquared(const Point3D& p1, const Point3D& p2) {
    return (p1 - p2).lengthSquared();
}*/

static inline coord_t distanceEuclidean(const Point3D& p1, const Point3D& p2) {
    return (p1 - p2).length();
}

static inline coord_t pointDot(const Point3D& p1, const Point3D& p2) {
    return p1.dot(p2);
}

static inline coord_t pointProj(const Point3D& p, const Point3D& n) {
    return p.dot(n) / n.dot(n);
}

static inline Point3D pointCross(const Point3D& p1, const Point3D& p2) {
    return p1.cross(p2);
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

static Point3D closestPointOnLineToPoint(const Point3D& l1, const Point3D& l2,
                                         const Point3D& p) {
    return Point3D::lerp(l1, clamp<coord_t>(0, pointProj(p - l1, l2 - l1), 1),
                         l2);
}

static coord_t closestDistanceToLineFromPoint(const Point3D& l1,
                                              const Point3D& l2,
                                              const Point3D& p) {
    return (closestPointOnLineToPoint(l1, l2, p) - p).length();
}

static Point3D ontoLineSegment(const Point3D& l1, const Point3D& x,
                               const Point3D& l2) {
    return Point3D::lerp(l1, clamp<coord_t>(0, pointProj(x - l1, l2 - l1), 1),
                         l2);
}

static Point3D closestPointOnLineToLine(const Point3D& t1, const Point3D& t2,
                                        const Point3D& o1, const Point3D& o2) {
    Point3D a = o2 - o1;
    Point3D b = t1 - o1;
    Point3D c = t2 - o1;
    b -= a * pointProj(b, a);
    c -= a * pointProj(c, a);
    Point3D d = c - b;
    coord_t t = d.lengthSquared() < 1e-9 ? 0 : pointDot(-b, d) / pointDot(d, d);
    Point3D l = Point3D::lerp(t1, clamp<coord_t>(0, t, 1), t2);
    return ontoLineSegment(t1, ontoLineSegment(o1, l, o2), t2);
}

static coord_t closestDistanceBetweenTwoLines(const Point3D& a1,
                                              const Point3D& a2,
                                              const Point3D& b1,
                                              const Point3D& b2) {
    return (closestPointOnLineToLine(a1, a2, b1, b2) -
            closestPointOnLineToLine(b1, b2, a1, a2))
        .length();
}

static bool collides2DLineLine(coord_t ax1, coord_t ay1, coord_t ax2,
                               coord_t ay2, coord_t bx1, coord_t by1,
                               coord_t bx2, coord_t by2) {
    coord_t d = (ax2 - ax1) * (by2 - by1) - (ay2 - ay1) * (bx2 - bx1);
    if (d == 0) {
        ax2 -= ax1, bx1 -= ax1, bx2 -= ax1;
        ay2 -= ay1, by1 -= ay1, by2 -= ay1;
        d = 1.0 / (ax2 * ax2 + ay2 * ay2);
        coord_t a = d * (ax2 * bx1 + ay2 * by1);
        coord_t b = d * (ax2 * bx2 + ay2 * by2);
        return collidesRangeRange(0, 1, a, b);
    }

    d = 1.0 / d;
    coord_t n = d * ((ay1 - by1) * (bx2 - bx1) - (ax1 - bx1) * (by2 - by1));
    coord_t m = d * ((ay1 - by1) * (ax2 - ax1) - (ax1 - bx1) * (ay2 - ay1));
    return 0 <= n && n <= 1 && 0 <= m && m <= 1;
}

static coord_t collides2DLineLineWhereUnlerp(coord_t ax1, coord_t ay1,
                                             coord_t ax2, coord_t ay2,
                                             coord_t bx1, coord_t by1,
                                             coord_t bx2, coord_t by2) {
    coord_t d = (ax2 - ax1) * (by2 - by1) - (ay2 - ay1) * (bx2 - bx1);
    if (d == 0) {
        ax2 -= ax1, bx1 -= ax1, bx2 -= ax1;
        ay2 -= ay1, by1 -= ay1, by2 -= ay1;
        d = 1.0 / (ax2 * ax2 + ay2 * ay2);
        coord_t a = d * (ax2 * bx1 + ay2 * by1);
        coord_t b = d * (ax2 * bx2 + ay2 * by2);
        return std::min(unlerp<coord_t>(a, 0, b), unlerp<coord_t>(a, 1, b));
    }

    return ((ay1 - by1) * (ax2 - ax1) - (ax1 - bx1) * (ay2 - ay1)) / d;
}

std::tuple<coord_t, coord_t, coord_t> pointRemapPlane(Point3D tx, Point3D ty,
                                                      Point3D tz, Point3D p) {
    // compute partial inverse matrix
    coord_t idet = 1.0 / (tx.x * (ty.y * tz.z - tz.y * ty.z) -
                          ty.x * (tx.y * tz.z - tz.y * tx.z) +
                          tz.x * (tx.y * ty.z - ty.y * tx.z));
    coord_t ixx = idet * (ty.y * tz.z - tz.y * ty.z);
    coord_t iyx = idet * (tz.x * ty.z - ty.x * tz.z);
    coord_t izx = idet * (ty.x * tz.y - tz.x * ty.y);
    coord_t ixy = idet * (tz.y * tx.z - tx.y * tz.z);
    coord_t iyy = idet * (tx.x * tz.z - tz.x * tx.z);
    coord_t izy = idet * (tz.x * tx.y - tx.x * tz.y);

    coord_t ixz = idet * (tx.y * ty.z - ty.y * tx.z);
    coord_t iyz = idet * (ty.x * tx.z - tx.x * ty.z);
    coord_t izz = idet * (tx.x * ty.y - ty.x * tx.y);
    return std::make_tuple(p.x * ixx + p.y * iyx + p.z * izx,
                           p.x * ixy + p.y * iyy + p.z * izy,
                           p.x * ixz + p.y * iyz + p.z * izz);
}

static coord_t closestDistanceToTriFromPoint(const Point3D& t1,
                                             const Point3D& t2,
                                             const Point3D& t3,
                                             const Point3D& p0) {
    Point3D x = t2 - t1;
    Point3D y = t3 - t1;
    auto [tx, ty, dz] =
        pointRemapPlane(x, y, pointCross(x, y).normalize(), p0 - t1);
    coord_t d, dx, dy;
    if (tx >= 0 || ty >= 0) {
        if (ty < tx - 1) {
            tx -= 1;
            d = sqrt(tx * tx + ty * ty);
        } else if (tx < ty - 1) {
            ty -= 1;
            d = sqrt(tx * tx + ty * ty);
        } else {
            d = std::max<coord_t>(0, tx + ty - 1) * numbers::ISQRT2<coord_t>;
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
    return sqrt(d * d + dz * dz);
}

bool collidesPointSphere(const Point3D& p, const Point3D& c, coord_t r) {
    return distanceEuclidean(p, c) <= r;
}

bool collidesLineSphere(const Point3D& l1, const Point3D& l2, const Point3D& c,
                        coord_t r) {
    return closestDistanceToLineFromPoint(l1, l2, c) <= r;
}

bool collidesPointCuboid(const Point3D& p, const Point3D& c1,
                         const Point3D& c2) {
    return collidesPointRange(p.x, c1.x, c2.x) &&
           collidesPointRange(p.y, c1.y, c2.y) &&
           collidesPointRange(p.z, c1.z, c2.z);
}

bool collidesLineCuboid(const Point3D& l1, const Point3D& l2, const Point3D& c1,
                        const Point3D& c2) {
    if (collidesPointCuboid(l1, c1, c2) && collidesPointCuboid(l2, c1, c2))
        return true;

    coord_t lx1 = unlerp(c1.x, l1.x, c2.x);
    coord_t lx2 = unlerp(c1.x, l2.x, c2.x);
    coord_t ly1 = unlerp(c1.y, l1.y, c2.y);
    coord_t ly2 = unlerp(c1.y, l2.y, c2.y);
    coord_t lz1 = unlerp(c1.z, l1.z, c2.z);
    coord_t lz2 = unlerp(c1.z, l2.z, c2.z);

    return (collidesPointRange(lx1, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, lx1, l2), c1, c2)) ||
           (collidesPointRange(lx2, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, lx2, l2), c1, c2)) ||
           (collidesPointRange(ly1, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, ly1, l2), c1, c2)) ||
           (collidesPointRange(ly2, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, ly2, l2), c1, c2)) ||
           (collidesPointRange(lz1, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, lz1, l2), c1, c2)) ||
           (collidesPointRange(lz2, 0, 1) &&
            collidesPointCuboid(Point3D::lerp(l1, lz2, l2), c1, c2));
}

bool collidesSphereCuboid(const Point3D& c, coord_t r, const Point3D& c1,
                          const Point3D& c2) {
    coord_t sx = clamp(c1.x, c.x, c2.x);
    coord_t sy = clamp(c1.y, c.y, c2.y);
    coord_t sz = clamp(c1.z, c.z, c2.z);
    return (Point3D(sx, sy, sz) - c).length() <= r;
}

bool collidesCuboidCuboid(const Point3D& a1, const Point3D& a2,
                          const Point3D& b1, const Point3D& b2) {
    return collidesRangeRange(a1.x, a2.x, b1.x, b2.x) &&
           collidesRangeRange(a1.y, a2.y, b1.y, b2.y) &&
           collidesRangeRange(a1.z, a2.z, b1.z, b2.z);
}

bool collidesSphereSphere(const Point3D& c1, coord_t r1, const Point3D& c2,
                          coord_t r2) {
    return distanceEuclidean(c1, c2) <= r1 + r2;
}

bool collidesLineTri(const Point3D& l1, const Point3D& l2, const Point3D& t1,
                     const Point3D& t2, const Point3D& t3) {
    Point3D a = l1 - t1;
    Point3D b = l2 - t1;
    Point3D x = t2 - t1;
    Point3D y = t3 - t1;
    Point3D z = pointCross(x, y).normalize();

    auto [ax, ay, az] = pointRemapPlane(x, y, z, a);
    auto [bx, by, bz] = pointRemapPlane(x, y, z, b);
    if (az * bz > pointRadius) return false;
    return (0 <= ax && ax <= 1 && 0 <= ay && ay <= 1 && ax + ay <= 1) ||
           (0 <= bx && bx <= 1 && 0 <= by && by <= 1 && bx + by <= 1) ||
           collides2DLineLine(ax, ay, bx, by, 0, 0, 1, 0) ||
           collides2DLineLine(ax, ay, bx, by, 0, 0, 0, 1) ||
           collides2DLineLine(ax, ay, bx, by, 1, 0, 0, 1);
}

bool collidesCuboidTri(const Point3D& c1, const Point3D& c2, const Point3D& t1,
                       const Point3D& t2, const Point3D& t3) {
    return collidesLineCuboid(t1, t2, c1, c2) &&
           collidesLineCuboid(t1, t3, c1, c2) &&
           collidesLineCuboid(t2, t3, c1, c2);
}

bool collidesSphereTri(const Point3D& c, coord_t r, const Point3D& t1,
                       const Point3D& t2, const Point3D& t3) {
    return closestDistanceToTriFromPoint(t1, t2, t3, c) <= r;
}

bool collidesCuboidSphere(const Point3D& cuboid1, const Point3D& cuboid2,
                          const Point3D& center, coord_t radius2) {
    return collidesSphereCuboid(center, radius2, cuboid1, cuboid2);
}

bool collidesCuboidPoint(const Point3D& cuboid1, const Point3D& cuboid2,
                         const Point3D& point) {
    return collidesPointCuboid(point, cuboid1, cuboid2);
}

bool collidesCuboidLine(const Point3D& cuboid1, const Point3D& cuboid2,
                        const Point3D& line1, const Point3D& line2) {
    return collidesLineCuboid(line1, line2, cuboid1, cuboid2);
}

bool collidesSpherePoint(const Point3D& center, coord_t radius,
                         const Point3D& point) {
    return collidesPointSphere(point, center, radius);
}

bool collidesSphereLine(const Point3D& center, coord_t radius,
                        const Point3D& line1, const Point3D& line2) {
    return collidesLineSphere(line1, line2, center, radius);
}

bool collidesTriTri(const Point3D& ta1, const Point3D& ta2, const Point3D& ta3,
                    const Point3D& tb1, const Point3D& tb2,
                    const Point3D& tb3) {
    Point3D a1 = ta2 - ta1;
    Point3D a2 = ta3 - ta1;
    Point3D b1 = tb1 - ta1;
    Point3D b2 = tb2 - ta2;
    Point3D b3 = tb3 - ta3;
    Point3D n = a1.cross(a2).normalize();
    coord_t b1d = n.dot(b1), b2d = n.dot(b2), b3d = n.dot(b3);

    if (sgn(b1d) != 0 && sgn(b1d) == sgn(b2d) && sgn(b1d) == sgn(b3d) &&
        sgn(b2d) == sgn(b3d))
        return false;

    if (abs(b1d) < pointRadius &&
        collidesSphereTri(tb1, pointRadius, ta1, ta2, ta3))
        return true;
    if (abs(b2d) < pointRadius &&
        collidesSphereTri(tb2, pointRadius, ta1, ta2, ta3))
        return true;
    if (abs(b3d) < pointRadius &&
        collidesSphereTri(tb3, pointRadius, ta1, ta2, ta3))
        return true;

    return collidesLineTri(tb1, tb2, ta1, ta2, ta3) ||
           collidesLineTri(tb1, tb3, ta1, ta2, ta3) ||
           collidesLineTri(tb2, tb3, ta1, ta2, ta3) ||
           collidesLineTri(ta1, ta2, tb1, tb2, tb3) ||
           collidesLineTri(ta1, ta3, tb1, tb2, tb3) ||
           collidesLineTri(ta2, ta3, tb1, tb2, tb3);
}

bool collidesSweepSpherePoint(const Point3D& c1, const Point3D& c2, coord_t r,
                              const Point3D& p) {
    return closestDistanceToLineFromPoint(c1, c2, p) < r;
}

static bool collidesSweepSphereLine(const Point3D& c1, const Point3D& c2,
                                    coord_t r, const Point3D& l1,
                                    const Point3D& l2) {
    return closestDistanceBetweenTwoLines(c1, c2, l1, l2) < r;
}

static bool collidesSweepSphereCuboid(const Point3D& c1, const Point3D& c2,
                                      coord_t r, const Point3D& q1,
                                      const Point3D& q2) {
    if (collidesSphereCuboid(c1, r, q1, q2) ||
        collidesSphereCuboid(c2, r, q1, q2))
        return true;
    Point3D cx = c2 - c1;
    Point3D qx = Point3D::average(q1, q2) - c1;
    Point3D qc = qx - cx;
    qc -= cx * cx.dot(qc);
    if (qc.length() > r) qc = qc.normalize() * r;
    return collidesLineCuboid(c1 + qc, c2 + qc, q1, q2);
}

static bool collidesSweepSphereSphere(const Point3D& c1, const Point3D& c2,
                                      coord_t r, const Point3D& sc,
                                      coord_t sr) {
    return closestDistanceToLineFromPoint(c1, c2, sc) < r + sr;
}

static bool collidesSweepSphereTri(const Point3D& c1, const Point3D& c2,
                                   coord_t r, const Point3D& t1,
                                   const Point3D& t2, const Point3D& t3) {
    Point3D tx = (t1 + t2 + t3) * (1.0 / 3);
    Point3D cd = c2 - c1;
    Point3D td = tx - c1;
    td -= cd * pointProj(td, cd);
    if (td.length() > r) td = td.normalize() * r;
    return collidesSphereTri(c1, r, t1, t2, t3) ||
           collidesSphereTri(c2, r, t1, t2, t3) ||
           collidesTriTri(c1, c1 + td, c2, t1, t2, t3) ||
           collidesTriTri(c1 + td, c2, c2 + td, t1, t2, t3);
}

static Point3D collidesSweepSpherePointWhere(const Point3D& l1,
                                             const Point3D& l2, coord_t r,
                                             const Point3D& p) {
    return p;
}

static Point3D collidesSweepSphereLineWhere(const Point3D& l1,
                                            const Point3D& l2, coord_t r,
                                            const Point3D& a1,
                                            const Point3D& a2) {
    return closestPointOnLineToLine(l1, l2, a1, a2);
}

Point3D collidesSweepSphereSphereWhere(const Point3D& l1, const Point3D& l2,
                                       coord_t r, const Point3D& sc,
                                       coord_t sr) {
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
    return Point3D::lerp(l1, std::min(q * (-b + d), q * (-b - d)), l2);
}

static coord_t validateCuboidUnlerp(coord_t x) { return x < 0 ? 1 : x; }

Point3D collidesSweepSphereCuboidWhere(const Point3D& l1, const Point3D& l2,
                                       coord_t r, const Point3D& c1,
                                       const Point3D& c2) {
    coord_t tx = validateCuboidUnlerp(l2.x > l1.x ? unlerp(l1.x, c1.x, l2.x)
                                                  : unlerp(l1.x, c2.x, l2.x));
    coord_t ty = validateCuboidUnlerp(l2.y > l1.y ? unlerp(l1.y, c1.y, l2.y)
                                                  : unlerp(l1.y, c2.y, l2.y));
    coord_t tz = validateCuboidUnlerp(l2.z > l1.z ? unlerp(l1.z, c1.z, l2.z)
                                                  : unlerp(l1.z, c2.z, l2.z));
    return Point3D::lerp(l1, std::min({tx, ty, tz}), l2);
}

static Point3D collidesSweepSphereTriWhere(const Point3D& c1, const Point3D& c2,
                                           coord_t r, const Point3D& t1,
                                           const Point3D& t2,
                                           const Point3D& t3) {
    Point3D tx = (t1 + t2 + t3) * (1.0 / 3);
    Point3D cx = Point3D::average(c1, c2);
    Point3D cd = c2 - c1;
    Point3D td = tx - cx;
    td -= cd * pointProj(td, cd);
    if (td.length() > r) td = td.normalize() * r;
    if (collidesSphereTri(c1, r, t1, t2, t3) &&
        !collidesSphereTri(c1, pointRadius, t1, t2, t3))
        return c1;
    if (collidesSphereTri(c2, r, t1, t2, t3) &&
        !collidesSphereTri(c2, pointRadius, t1, t2, t3))
        return c2;

    Point3D a = c1 - t1;
    Point3D b = c2 - t1;
    Point3D x = t2 - t1;
    Point3D y = t3 - t1;
    Point3D z = pointCross(x, y).normalize();

    auto [ax, ay, az] = pointRemapPlane(x, y, z, a);
    auto [bx, by, bz] = pointRemapPlane(x, y, z, b);
    (void)az;
    (void)bz;
    if (0 <= ax && ax <= 1 && 0 <= ay && ay <= 1 && ax + ay <= 1) return c1;
    if (collides2DLineLine(ax, ay, bx, by, 0, 0, 1, 0))
        return Point3D::lerp(
            c1, collides2DLineLineWhereUnlerp(ax, ay, bx, by, 0, 0, 1, 0), c2);
    if (collides2DLineLine(ax, ay, bx, by, 0, 0, 0, 1))
        return Point3D::lerp(
            c1, collides2DLineLineWhereUnlerp(ax, ay, bx, by, 0, 0, 0, 1), c2);
    if (collides2DLineLine(ax, ay, bx, by, 1, 0, 0, 1))
        return Point3D::lerp(
            c1, collides2DLineLineWhereUnlerp(ax, ay, bx, by, 1, 0, 0, 1), c2);
    return c1;
}

bool collidesPointShape(const Point3D& p, const CollisionShape& shape,
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

bool collidesLineShape(const Point3D& p1, const Point3D& p2,
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

bool collidesCuboidShape(const Point3D& c1, const Point3D& c2,
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

bool collidesSphereShape(const Point3D& c, coord_t r,
                         const CollisionShape& shape, const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSpherePoint(c, r, mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSphereLine(c, r, mat.project(shape.p),
                                      mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSphereCuboid(c, r, mat.project(shape.p),
                                        mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSphereSphere(c, r, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesSphereTri(c, r, mat.project(shape.p),
                                     mat.project(shape.p1),
                                     mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

static bool collidesTriShape(const Point3D& t0, const Point3D& t1,
                             const Point3D& t2, const CollisionShape& shape,
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

bool collidesPointModel(const Point3D& p, const ModelCollision& mc,
                        const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesPointShape(p, shape, mat);
                       });
}

bool collidesLineModel(const Point3D& p1, const Point3D& p2,
                       const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesLineShape(p1, p2, shape, mat);
                       });
}

bool collidesCuboidModel(const Point3D& c1, const Point3D& c2,
                         const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesCuboidShape(c1, c2, shape, mat);
                       });
}

bool collidesSphereModel(const Point3D& c, coord_t r2, const ModelCollision& mc,
                         const Matrix3D& mat) {
    return std::any_of(mc.shapes.begin(), mc.shapes.end(),
                       [&](const CollisionShape& shape) {
                           return collidesSphereShape(c, r2, shape, mat);
                       });
}

static bool collidesTriModel(const Point3D& t0, const Point3D& t1,
                             const Point3D& t2, const ModelCollision& mc,
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

static bool collidesSweepSphereShape(const Point3D& c1, const Point3D& c2,
                                     coord_t r, const CollisionShape& shape,
                                     const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSweepSpherePoint(c1, c2, r, mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSweepSphereLine(c1, c2, r, mat.project(shape.p),
                                           mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSweepSphereCuboid(c1, c2, r, mat.project(shape.p),
                                             mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSweepSphereSphere(c1, c2, r, mat.project(shape.p),
                                             shape.r);
        case CollisionShapeType::Tri:
            return collidesSweepSphereTri(c1, c2, r, mat.project(shape.p),
                                          mat.project(shape.p1),
                                          mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

static Point3D collidesSweepSphereShapeWhere(const Point3D& c1,
                                             const Point3D& c2, coord_t r,
                                             const CollisionShape& shape,
                                             const Matrix3D& mat) {
    switch (shape.type) {
        case CollisionShapeType::Point:
            return collidesSweepSpherePointWhere(c1, c2, r,
                                                 mat.project(shape.p));
        case CollisionShapeType::Line:
            return collidesSweepSphereLineWhere(c1, c2, r, mat.project(shape.p),
                                                mat.project(shape.p1));
        case CollisionShapeType::Cuboid:
            return collidesSweepSphereCuboidWhere(
                c1, c2, r, mat.project(shape.p), mat.project(shape.p1));
        case CollisionShapeType::Sphere:
            return collidesSweepSphereSphereWhere(
                c1, c2, r, mat.project(shape.p), shape.r);
        case CollisionShapeType::Tri:
            return collidesSweepSphereTriWhere(c1, c2, r, mat.project(shape.p),
                                               mat.project(shape.p1),
                                               mat.project(shape.p2));
        default:
            never("invalid shape");
    }
}

bool collidesSweepSphereModel(const Point3D& c1, const Point3D& c2, coord_t r,
                              const ModelCollision& mc, const Matrix3D& mat) {
    return std::any_of(
        mc.shapes.begin(), mc.shapes.end(), [&](const CollisionShape& shape) {
            return collidesSweepSphereShape(c1, c2, r, shape, mat);
        });
}

Point3D collidesSweepSphereModelWhere(const Point3D& l1, const Point3D& l2,
                                      coord_t r, const ModelCollision& mc,
                                      const Matrix3D& mat) {
    for (const CollisionShape& shape : mc.shapes) {
        if (collidesSweepSphereShape(l1, l2, r, shape, mat))
            return collidesSweepSphereShapeWhere(l1, l2, r, shape, mat);
    }
    return l1;
}

Point3D collidesCuboidPointDirection(const Point3D& them, const Point3D& me,
                                     const Point3D& mySize) {
    coord_t dx = (them.x - me.x) / mySize.x;
    coord_t dy = (them.y - me.y) / mySize.y;
    coord_t dz = (them.z - me.z) / mySize.z;
    if (std::abs(dz) >= std::abs(dx) && std::abs(dz) >= std::abs(dy))
        dx = dy = 0;
    else if (std::abs(dx) >= std::abs(dy) && std::abs(dx) >= std::abs(dz))
        dy = dz = 0;
    else if (std::abs(dy) >= std::abs(dx) && std::abs(dy) >= std::abs(dz))
        dx = dz = 0;
    return Point3D(dx, dy, dz);
}

}  // namespace hiemalia
