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

#include "logger.hh"
#include "math.hh"

namespace hiemalia {
static inline coord_t lengthSquared(const ModelPoint& p) {
    return p.x * p.x + p.y * p.y + p.z * p.z;
}

static inline coord_t distanceSquared(const ModelPoint& p1,
                                      const ModelPoint& p2) {
    coord_t x = p1.x - p2.x;
    coord_t y = p1.y - p2.y;
    coord_t z = p1.z - p2.z;
    return x * x + y * y + z * z;
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

static coord_t closestDistanceToLineFromPoint(const ModelPoint& l1,
                                              const ModelPoint& l2,
                                              const ModelPoint& p) {
    ModelPoint p1 = l2 - l1;
    ModelPoint p2 = p - l1;
    if (pointDot(p2, p1) <= 0) return lengthSquared(p2);

    ModelPoint p3 = p - l2;
    if (pointDot(p3, p1) >= 0) return lengthSquared(p3);

    return lengthSquared(pointCross(p1, p2)) / lengthSquared(p1);
}

bool collidesPointSphere(const ModelPoint& p, const ModelPoint& c, coord_t r2) {
    return distanceSquared(p, c) <= r2;
}

bool collidesLineSphere(const ModelPoint& l1, const ModelPoint& l2,
                        const ModelPoint& c, coord_t r2) {
    return closestDistanceToLineFromPoint(l1, l2, c) <= r2;
}

bool collidesPointCuboid(const ModelPoint& p, const ModelPoint& c1,
                         const ModelPoint& c2) {
    return collidesPointRange(p.x, c1.x, c2.x) &&
           collidesPointRange(p.y, c1.y, c2.y) &&
           collidesPointRange(p.z, c1.z, c2.z);
}

bool collidesLineCuboid(const ModelPoint& l1, const ModelPoint& l2,
                        const ModelPoint& c1, const ModelPoint& c2) {
    coord_t lx1 = unlerp(c1.x, l1.x, c2.x);
    coord_t lx2 = unlerp(c1.x, l2.x, c2.x);
    coord_t ly1 = unlerp(c1.y, l1.y, c2.y);
    coord_t ly2 = unlerp(c1.y, l2.y, c2.y);
    coord_t lz1 = unlerp(c1.z, l1.z, c2.z);
    coord_t lz2 = unlerp(c1.z, l2.z, c2.z);

    if (0 <= lx1 && lx1 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, lx1, l2), c1, c2))
        return true;
    if (0 <= lx2 && lx2 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, lx2, l2), c1, c2))
        return true;
    if (0 <= ly1 && ly1 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, ly1, l2), c1, c2))
        return true;
    if (0 <= ly2 && ly2 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, ly2, l2), c1, c2))
        return true;
    if (0 <= lz1 && lz1 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, lz1, l2), c1, c2))
        return true;
    if (0 <= lz2 && lz2 <= 1 &&
        collidesPointCuboid(lerpPoint(l1, lz2, l2), c1, c2))
        return true;
    return false;
}

bool collidesSphereCuboid(const ModelPoint& c, coord_t r2, const ModelPoint& c1,
                          const ModelPoint& c2) {
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

bool collidesCuboidSphere(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                          const ModelPoint& center, coord_t radius2) {
    return collidesSphereCuboid(center, radius2, cuboid1, cuboid2);
}

bool collidesCuboidCuboid(const ModelPoint& a1, const ModelPoint& a2,
                          const ModelPoint& b1, const ModelPoint& b2) {
    return collidesRangeRange(a1.x, a2.x, b1.x, b2.x) &&
           collidesRangeRange(a1.y, a2.y, b1.y, b2.y) &&
           collidesRangeRange(a1.z, a2.z, b1.z, b2.z);
}

bool collidesSphereSphere(const ModelPoint& c1, coord_t r1,
                          const ModelPoint& c2, coord_t r2) {
    return distanceSquared(c1, c2) <= r1 + r2;
}

bool collidesLineTri(const ModelPoint& l1, const ModelPoint& l2,
                     const ModelPoint& t1, const ModelPoint& t2,
                     const ModelPoint& t3) {
    ModelPoint a = l1 - t1;
    ModelPoint b = l2 - t1;
    ModelPoint x = t2 - t1;
    ModelPoint y = t3 - t1;
    ModelPoint z = pointCross(x, y);

    coord_t c1 = pointDot(a, z);
    coord_t c2 = pointDot(b, z);
    if (c1 * c2 > 0) return false;

    ModelPoint p = lerpPoint(a, unlerp<coord_t>(c1, 0, c2), b);
    coord_t tx = pointDot(p, x);
    coord_t ty = pointDot(p, y);
    return 0 <= tx && tx <= 1 && 0 <= ty && ty <= 1 && tx + ty <= 1;
}

bool collidesSphereTri(const ModelPoint& c, coord_t r2, const ModelPoint& t1,
                       const ModelPoint& t2, const ModelPoint& t3) {
    return collidesLineSphere(t1, t2, c, r2) &&
           collidesLineSphere(t1, t3, c, r2) &&
           collidesLineSphere(t2, t3, c, r2);
}

ModelPoint collidesLineSphereWhere(const ModelPoint& l1, const ModelPoint& l2,
                                   const ModelPoint& sc, coord_t r2) {
    coord_t a = pointDot(l2, l2);
    coord_t b = 2 * pointDot(l2, l1 - sc);
    coord_t c = pointDot(l1 - sc, l1 - sc) - r2;
    coord_t d = b * b - 4 * a * c;
    if (d < 0) {
        LOG_WARN("tried to find intersection point, but there is none");
        return l1;
    }
    coord_t q = 0.5 / a;
    d = std::sqrt(d);
    return lerpPoint(l1, std::min(q * (-b + d), q * (-b - d)), l2);
}

static coord_t validateCuboidUnlerp(coord_t x) { return x < 0 ? 1 : x; }

ModelPoint collidesLineCuboidWhere(const ModelPoint& l1, const ModelPoint& l2,
                                   const ModelPoint& c1, const ModelPoint& c2) {
    coord_t tx = validateCuboidUnlerp(l2.x > l1.x ? unlerp(l1.x, c1.x, l2.x)
                                                  : unlerp(l1.x, c2.x, l2.x));
    coord_t ty = validateCuboidUnlerp(l2.y > l1.y ? unlerp(l1.y, c1.y, l2.y)
                                                  : unlerp(l1.y, c2.y, l2.y));
    coord_t tz = validateCuboidUnlerp(l2.z > l1.z ? unlerp(l1.z, c1.z, l2.z)
                                                  : unlerp(l1.z, c2.z, l2.z));
    return lerpPoint(l1, std::min({tx, ty, tz}), l2);
}

}  // namespace hiemalia
