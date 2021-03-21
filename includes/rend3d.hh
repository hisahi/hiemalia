/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rend3d.hh: header file for rend3d.cc

#ifndef M_REND3D_HH
#define M_REND3D_HH

#include <string>
#include <vector>

#include "defs.hh"
#include "model.hh"
#include "sbuf.hh"

namespace hiemalia {
struct Rotation3D {
    coord_t yaw;
    coord_t pitch;
    coord_t roll;

    Rotation3D(coord_t yaw, coord_t pitch, coord_t roll)
        : yaw(yaw), pitch(pitch), roll(roll) {}
};

struct Matrix3D;

struct Vector3D {
    coord_t x;
    coord_t y;
    coord_t z;
    coord_t w;

    Vector3D(const ModelPoint& p) : x(p.x), y(p.y), z(p.z), w(1) {}
    Vector3D(coord_t x, coord_t y, coord_t z) : x(x), y(y), z(z), w(1) {}
    Vector3D(coord_t x, coord_t y, coord_t z, coord_t w)
        : x(x), y(y), z(z), w(w) {}

    inline Vector3D& operator+=(const Vector3D& r) {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    inline Vector3D& operator-=(const Vector3D& r) {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        return *this;
    }

    friend inline Vector3D operator+(Vector3D a, const Vector3D& b) {
        return a += b;
    }
    friend inline Vector3D operator-(Vector3D a, const Vector3D& b) {
        return a -= b;
    }

    inline coord_t getX() { return x / w; }
    inline coord_t getY() { return y / w; }
    inline coord_t getDepth() { return z / w; }

    ModelPoint toCartesian() const;

    inline static coord_t dot(const Vector3D& a, const Vector3D& b) {
        coord_t wf = 1.0 / (a.w * b.w);
        return wf * a.x * b.x + wf * a.y * b.y + wf * a.z * b.z;
    }
    inline static Vector3D cross(const Vector3D& a, const Vector3D& b) {
        coord_t wf = 1.0 / (a.w * b.w);
        return Vector3D(wf * a.y * b.z - wf * a.z * b.y,
                        wf * a.x * b.z - wf * a.z * b.x,
                        wf * a.x * b.y - wf * a.y * b.x, 1);
    }

    Vector3D project(const Matrix3D& m);
};

struct Matrix3D {
    coord_t m[16];

    Matrix3D() : m{0} {}
    Matrix3D(coord_t a00, coord_t a01, coord_t a02, coord_t a03, coord_t a10,
             coord_t a11, coord_t a12, coord_t a13, coord_t a20, coord_t a21,
             coord_t a22, coord_t a23, coord_t a30, coord_t a31, coord_t a32,
             coord_t a33)
        : m{a00, a01, a02, a03, a10, a11, a12, a13,
            a20, a21, a22, a23, a30, a31, a32, a33} {}

    inline coord_t& operator[](size_t i) { return m[i]; }
    inline const coord_t& operator[](size_t i) const { return m[i]; }

    inline static Matrix3D identity() {
        return Matrix3D({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
    }

    static Matrix3D translate(const ModelPoint& p);
    static Matrix3D rotate(const Rotation3D& r);
    static Matrix3D scale(coord_t s);
    static Matrix3D yaw(coord_t theta);
    static Matrix3D pitch(coord_t theta);
    static Matrix3D roll(coord_t theta);

    inline bool operator==(const Matrix3D& r) const {
        for (size_t i = 0; i < 16; ++i)
            if (m[i] != r.m[i]) return false;
        return true;
    }

    inline bool operator!=(const Matrix3D& r) const { return !(*this == r); }

    inline Matrix3D& operator+=(const Matrix3D& r) {
        m[0] += r[0];
        m[1] += r[1];
        m[2] += r[2];
        m[3] += r[3];
        m[4] += r[4];
        m[5] += r[5];
        m[6] += r[6];
        m[7] += r[7];
        m[8] += r[8];
        m[9] += r[9];
        m[10] += r[10];
        m[11] += r[11];
        m[12] += r[12];
        m[13] += r[13];
        m[14] += r[14];
        m[15] += r[15];
        return *this;
    }
    inline Matrix3D& operator-=(const Matrix3D& r) {
        m[0] -= r[0];
        m[1] -= r[1];
        m[2] -= r[2];
        m[3] -= r[3];
        m[4] -= r[4];
        m[5] -= r[5];
        m[6] -= r[6];
        m[7] -= r[7];
        m[8] -= r[8];
        m[9] -= r[9];
        m[10] -= r[10];
        m[11] -= r[11];
        m[12] -= r[12];
        m[13] -= r[13];
        m[14] -= r[14];
        m[15] -= r[15];
        return *this;
    }
    inline Matrix3D& operator*=(const Matrix3D& r) {
        *this = *this * r;
        return *this;
    }

    inline Matrix3D operator-() const {
        Matrix3D a;
        a[0] = -m[0];
        a[1] = -m[1];
        a[2] = -m[2];
        a[3] = -m[3];
        a[4] = -m[4];
        a[5] = -m[5];
        a[6] = -m[6];
        a[7] = -m[7];
        a[8] = -m[8];
        a[9] = -m[9];
        a[10] = -m[10];
        a[11] = -m[11];
        a[12] = -m[12];
        a[13] = -m[13];
        a[14] = -m[14];
        a[15] = -m[15];
        return a;
    }

    friend inline Matrix3D operator+(Matrix3D a, const Matrix3D& b) {
        return a += b;
    }
    friend inline Matrix3D operator-(Matrix3D a, const Matrix3D& b) {
        return a -= b;
    }
    friend inline Matrix3D operator*(const Matrix3D& a, const Matrix3D& b) {
        Matrix3D r;
        r[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
        r[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
        r[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
        r[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
        r[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
        r[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
        r[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
        r[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];
        r[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
        r[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
        r[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
        r[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];
        r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
        r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
        r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
        r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
        return r;
    }
};

#if !NDEBUG
std::string printMatrix(const Matrix3D& m);  // test.cc
#endif

struct ScreenPoint {
    Vector3D v;
    bool onScreen;
    bool inFront;

    ScreenPoint(const Vector3D& v)
        : v(v), onScreen(screenCheck(v)), inFront(v.z >= 0) {}

    inline ModelPoint toCartesian() const { return v.toCartesian(); }

   private:
    inline static bool screenCheck(const Vector3D& v) {
        return v.x >= -1 && v.x <= 1 && v.y >= -1 && v.y <= 1 && v.z >= 0 &&
               v.z <= 1;
    }
};

class Renderer3D {
   public:
    Matrix3D getModelMatrix(ModelPoint p, Rotation3D r, coord_t s) const;
    void renderModel(SplinterBuffer& buf, ModelPoint p, Rotation3D r, coord_t s,
                     const Model& m);
    void setCamera(ModelPoint pos, Rotation3D rot);
    Renderer3D();

   private:
    void renderModelFragment(SplinterBuffer& buf, const ModelFragment& f) const;
    void projectVertices(const std::vector<ModelPoint>& v, const Matrix3D& m);
    ModelPoint clip(const ScreenPoint& onScreen,
                    const ScreenPoint& offScreen) const;
    Matrix3D view;
    std::vector<ScreenPoint> points_;
};
};  // namespace hiemalia

#endif  // M_REND3D_HH
