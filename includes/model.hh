/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// model.hh: header file for 3D models

#ifndef M_MODEL_HH
#define M_MODEL_HH

#include <vector>

#include "defs.hh"
#include "math.hh"
#include "sbuf.hh"

namespace hiemalia {
struct Point3D {
    coord_t x;
    coord_t y;
    coord_t z;

    Point3D(coord_t x, coord_t y, coord_t z) : x(x), y(y), z(z) {}

    inline Point3D operator-() const { return Point3D(-x, -y, -z); }

    inline Point3D& operator+=(const Point3D& r) noexcept {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    inline Point3D& operator-=(const Point3D& r) noexcept {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    inline Point3D& operator*=(const coord_t& f) noexcept {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    friend inline Point3D operator+(Point3D a, const Point3D& b) noexcept {
        return a += b;
    }
    friend inline Point3D operator-(Point3D a, const Point3D& b) noexcept {
        return a -= b;
    }
    friend inline Point3D operator*(const coord_t& f, Point3D a) noexcept {
        return a *= f;
    }
    friend inline Point3D operator*(Point3D a, const coord_t& f) noexcept {
        return a *= f;
    }

    inline bool operator==(const Point3D& b) const noexcept {
        return x == b.x && y == b.y && z == b.z;
    }
    inline bool operator!=(const Point3D& b) const noexcept {
        return !(*this == b);
    }

    inline bool isZero() const noexcept { return x == 0 && y == 0 && z == 0; }
    inline explicit operator bool() const noexcept { return !isZero(); }
    inline bool operator!() const noexcept { return isZero(); }

    inline coord_t dot(const Point3D& b) const noexcept {
        return x * b.x + y * b.y + z * b.z;
    }
    inline Point3D cross(const Point3D& b) const noexcept {
        return Point3D(y * b.z - z * b.y, x * b.z - z * b.x, x * b.y - y * b.x);
    }

    inline static Point3D average(const Point3D& a, const Point3D& b) noexcept {
        return Point3D(0.5 * (a.x + b.x), 0.5 * (a.y + b.y), 0.5 * (a.z + b.z));
    }
    inline static Point3D lerp(const Point3D& a, coord_t t,
                               const Point3D& b) noexcept {
        return Point3D(hiemalia::lerp(a.x, t, b.x), hiemalia::lerp(a.y, t, b.y),
                       hiemalia::lerp(a.z, t, b.z));
    }
    inline static Point3D sqerp(const Point3D& a, coord_t t,
                                const Point3D& b) noexcept {
        return Point3D(hiemalia::sqerp(a.x, t, b.x),
                       hiemalia::sqerp(a.y, t, b.y),
                       hiemalia::sqerp(a.z, t, b.z));
    }
    inline coord_t lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    inline coord_t length() const noexcept { return sqrt(lengthSquared()); }
    inline Point3D normalize() const {
        dynamic_assert(x != 0 || y != 0 || z != 0,
                       "attempt to normalize zero vector!");
        return *this * (1 / sqrt(lengthSquared()));
    }

    static Point3D origin;
};

inline Point3D Point3D::origin = Point3D(0, 0, 0);

struct Orient3D {
    coord_t yaw;
    coord_t pitch;
    coord_t roll;

    Orient3D(coord_t yaw, coord_t pitch, coord_t roll)
        : yaw(yaw), pitch(pitch), roll(roll) {}

    inline Orient3D& operator+=(const Orient3D& r) {
        yaw += r.yaw;
        pitch += r.pitch;
        roll += r.roll;
        wrap();
        return *this;
    }
    inline Orient3D& operator-=(const Orient3D& r) {
        yaw -= r.yaw;
        pitch -= r.pitch;
        roll -= r.roll;
        wrap();
        return *this;
    }
    inline Orient3D& operator*=(const coord_t& f) {
        yaw *= f;
        pitch *= f;
        roll *= f;
        wrap();
        return *this;
    }

    friend inline Orient3D operator+(Orient3D a, const Orient3D& b) {
        return a += b;
    }
    friend inline Orient3D operator-(Orient3D a, const Orient3D& b) {
        return a -= b;
    }
    friend inline Orient3D operator*(Orient3D a, const coord_t& f) {
        return a *= f;
    }

    inline bool isZero() const noexcept {
        return yaw == 0 && pitch == 0 && roll == 0;
    }
    inline explicit operator bool() const noexcept { return !isZero(); }
    inline bool operator!() const noexcept { return isZero(); }

    inline void wrap() noexcept {
        yaw = wrapAngle(yaw);
        pitch = wrapAngle(pitch);
        roll = wrapAngle(roll);
    }

    inline bool operator==(const Orient3D& b) const {
        return wrapAngle(yaw) == wrapAngle(b.yaw) &&
               wrapAngle(pitch) == wrapAngle(b.pitch) &&
               wrapAngle(roll) == wrapAngle(b.roll);
    }
    inline bool operator!=(const Orient3D& b) const { return !(*this == b); }

    // implemented in rend3d.cc
    coord_t offBy(const Orient3D& other) const noexcept;
    Point3D rotate(const Point3D& vector, coord_t scale = 1) const noexcept;
    static Orient3D toPolar(const Point3D& p, coord_t roll = 0) noexcept;
    Orient3D tendTo(const Orient3D& target, coord_t rate) const noexcept;

    static Orient3D atScreen;
    static Orient3D atPlayer;
};

inline Orient3D Orient3D::atScreen = Orient3D{0, 0, 0};
inline Orient3D Orient3D::atPlayer = Orient3D{numbers::PI<coord_t>, 0, 0};

struct ModelFragment {
    Color color;
    size_t start;
    std::vector<size_t> points;

    ModelFragment(const Color& color, size_t start)
        : color(color), start(start), points() {}
    ModelFragment(const Color& color, size_t start,
                  std::initializer_list<size_t> points)
        : color(color), start(start), points(points) {}
    ModelFragment(const Color& color, size_t start,
                  std::vector<size_t>&& points) noexcept
        : color(color), start(start), points(std::move(points)) {}
};

struct Model {
    std::vector<Point3D> vertices;
    std::vector<ModelFragment> shapes;

    Model() : vertices(), shapes() {}
    Model(std::vector<Point3D>&& vertices,
          std::vector<ModelFragment>&& shapes) noexcept
        : vertices(std::move(vertices)), shapes(std::move(shapes)) {}
};

};  // namespace hiemalia

#endif  // M_MODEL_HH
