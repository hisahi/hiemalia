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

static inline ModelPoint lerpPoint(const ModelPoint& p1, coord_t t,
                                   const ModelPoint& p2) {
    return ModelPoint(lerp(p1.x, t, p2.x), lerp(p1.y, t, p2.y),
                      lerp(p1.z, t, p2.z));
}

static inline ModelPoint pointAdd(const ModelPoint& p1, const ModelPoint& p2) {
    return ModelPoint(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

static inline ModelPoint pointSub(const ModelPoint& p1, const ModelPoint& p2) {
    return ModelPoint(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

static inline coord_t pointDot(const ModelPoint& p1, const ModelPoint& p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

static inline ModelPoint pointCross(const ModelPoint& p1,
                                    const ModelPoint& p2) {
    return ModelPoint(p1.y * p2.z - p1.z * p1.y, p1.x * p2.z - p1.z * p1.x,
                      p1.x * p2.y - p1.y * p1.x);
}

static bool collidesRangeRange(coord_t x1, coord_t x2, coord_t y1, coord_t y2) {
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    return x1 <= y2 && y1 <= x2;
}

static coord_t closestDistanceToLineFromPoint(const ModelPoint& l1,
                                              const ModelPoint& l2,
                                              const ModelPoint& p) {
    ModelPoint p1 = pointSub(l2, l1);
    ModelPoint p2 = pointSub(p, l1);
    if (pointDot(p2, p1) <= 0) return lengthSquared(p2);

    ModelPoint p3 = pointSub(p, l2);
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
    return p.x >= std::min(c1.x, c2.x) && p.x <= std::max(c1.x, c2.x) &&
           p.y >= std::min(c1.y, c2.y) && p.y <= std::max(c1.y, c2.y) &&
           p.z >= std::min(c1.z, c2.z) && p.z <= std::max(c1.z, c2.z);
}

bool collidesLineCuboid(const ModelPoint& l1, const ModelPoint& l2,
                        const ModelPoint& c1, const ModelPoint& c2) {
    coord_t lx1 = (std::min(c1.x, c2.x) - l1.x) / (l2.x - l1.x);
    coord_t lx2 = (std::max(c1.x, c2.x) - l1.x) / (l2.x - l1.x);
    coord_t ly1 = (std::min(c1.y, c2.y) - l1.y) / (l2.y - l1.y);
    coord_t ly2 = (std::max(c1.y, c2.y) - l1.y) / (l2.y - l1.y);
    coord_t lz1 = (std::min(c1.z, c2.z) - l1.z) / (l2.z - l1.z);
    coord_t lz2 = (std::max(c1.z, c2.z) - l1.z) / (l2.z - l1.z);

    coord_t a1 = std::max({lx1, ly1, lz1});
    coord_t a2 = std::min({lx2, ly2, lz2});
    return a1 <= a2 && a1 <= 1 && a2 >= 0;
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
    ModelPoint a = pointSub(l1, t1);
    ModelPoint b = pointSub(l2, t1);
    ModelPoint x = pointSub(t2, t1);
    ModelPoint y = pointSub(t3, t1);
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
}  // namespace hiemalia
