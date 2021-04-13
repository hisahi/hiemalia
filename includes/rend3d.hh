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
static const coord_t viewDistance = 2;

struct Vector3D {
    coord_t x;
    coord_t y;
    coord_t z;
    coord_t w;

    Vector3D(const Point3D& p) : x(p.x), y(p.y), z(p.z), w(1) {}
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

    Point3D toCartesian() const;

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
};

struct Matrix3D3 {
    coord_t m[9];

    Matrix3D3() : m{0} {}
    Matrix3D3(coord_t a00, coord_t a01, coord_t a02, coord_t a10, coord_t a11,
              coord_t a12, coord_t a20, coord_t a21, coord_t a22)
        : m{a00, a01, a02, a10, a11, a12, a20, a21, a22} {}

    inline coord_t& operator[](size_t i) { return m[i]; }
    inline const coord_t& operator[](size_t i) const { return m[i]; }

    inline static Matrix3D3 identity() {
        return Matrix3D3({1, 0, 0, 0, 1, 0, 0, 0, 1});
    }

    static Matrix3D3 rotate(const Orient3D& r);
    static Matrix3D3 scale(const Point3D& s);
    static Matrix3D3 scale(coord_t s);
    static Matrix3D3 yaw(coord_t theta);
    static Matrix3D3 pitch(coord_t theta);
    static Matrix3D3 roll(coord_t theta);

    Point3D project(const Point3D& p) const;
    Vector3D project(const Vector3D& v) const;

    inline bool operator==(const Matrix3D3& r) const {
        for (size_t i = 0; i < 9; ++i)
            if (m[i] != r.m[i]) return false;
        return true;
    }

    inline bool operator!=(const Matrix3D3& r) const { return !(*this == r); }

    inline Matrix3D3& operator+=(const Matrix3D3& r) {
        m[0] += r[0];
        m[1] += r[1];
        m[2] += r[2];
        m[3] += r[3];
        m[4] += r[4];
        m[5] += r[5];
        m[6] += r[6];
        m[7] += r[7];
        m[8] += r[8];
        return *this;
    }
    inline Matrix3D3& operator-=(const Matrix3D3& r) {
        m[0] -= r[0];
        m[1] -= r[1];
        m[2] -= r[2];
        m[3] -= r[3];
        m[4] -= r[4];
        m[5] -= r[5];
        m[6] -= r[6];
        m[7] -= r[7];
        m[8] -= r[8];
        return *this;
    }
    inline Matrix3D3& operator*=(const Matrix3D3& r) {
        *this = *this * r;
        return *this;
    }

    inline Matrix3D3 operator-() const {
        return Matrix3D3(-m[0], -m[1], -m[2], -m[3], -m[4], -m[5], -m[6], -m[7],
                         -m[8]);
    }

    friend inline Matrix3D3 operator+(Matrix3D3 a, const Matrix3D3& b) {
        return a += b;
    }
    friend inline Matrix3D3 operator-(Matrix3D3 a, const Matrix3D3& b) {
        return a -= b;
    }
    friend inline Matrix3D3 operator*(const Matrix3D3& a, const Matrix3D3& b) {
        return Matrix3D3(a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6],
                         a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7],
                         a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8],
                         a.m[3] * b.m[0] + a.m[4] * b.m[3] + a.m[5] * b.m[6],
                         a.m[3] * b.m[1] + a.m[4] * b.m[4] + a.m[5] * b.m[7],
                         a.m[3] * b.m[2] + a.m[4] * b.m[5] + a.m[5] * b.m[8],
                         a.m[6] * b.m[0] + a.m[7] * b.m[3] + a.m[8] * b.m[6],
                         a.m[6] * b.m[1] + a.m[7] * b.m[4] + a.m[8] * b.m[7],
                         a.m[6] * b.m[2] + a.m[7] * b.m[5] + a.m[8] * b.m[8]);
    }
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

    static Matrix3D translate(const Point3D& p);

    Point3D project(const Point3D& p) const;
    Vector3D project(const Vector3D& v) const;

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
    inline Matrix3D& operator*=(const Matrix3D3& r) {
        *this = *this * r;
        return *this;
    }

    inline Matrix3D operator-() const {
        return Matrix3D(-m[0], -m[1], -m[2], -m[3], -m[4], -m[5], -m[6], -m[7],
                        -m[8], -m[9], -m[10], -m[11], -m[12], -m[13], -m[14],
                        -m[15]);
    }

    friend inline Matrix3D operator+(Matrix3D a, const Matrix3D& b) {
        return a += b;
    }
    friend inline Matrix3D operator-(Matrix3D a, const Matrix3D& b) {
        return a -= b;
    }
    friend inline Matrix3D operator*(const Matrix3D& a, const Matrix3D& b) {
        return Matrix3D(a.m[0] * b.m[0] + a.m[1] * b.m[4] + a.m[2] * b.m[8] +
                            a.m[3] * b.m[12],
                        a.m[0] * b.m[1] + a.m[1] * b.m[5] + a.m[2] * b.m[9] +
                            a.m[3] * b.m[13],
                        a.m[0] * b.m[2] + a.m[1] * b.m[6] + a.m[2] * b.m[10] +
                            a.m[3] * b.m[14],
                        a.m[0] * b.m[3] + a.m[1] * b.m[7] + a.m[2] * b.m[11] +
                            a.m[3] * b.m[15],
                        a.m[4] * b.m[0] + a.m[5] * b.m[4] + a.m[6] * b.m[8] +
                            a.m[7] * b.m[12],
                        a.m[4] * b.m[1] + a.m[5] * b.m[5] + a.m[6] * b.m[9] +
                            a.m[7] * b.m[13],
                        a.m[4] * b.m[2] + a.m[5] * b.m[6] + a.m[6] * b.m[10] +
                            a.m[7] * b.m[14],
                        a.m[4] * b.m[3] + a.m[5] * b.m[7] + a.m[6] * b.m[11] +
                            a.m[7] * b.m[15],
                        a.m[8] * b.m[0] + a.m[9] * b.m[4] + a.m[10] * b.m[8] +
                            a.m[11] * b.m[12],
                        a.m[8] * b.m[1] + a.m[9] * b.m[5] + a.m[10] * b.m[9] +
                            a.m[11] * b.m[13],
                        a.m[8] * b.m[2] + a.m[9] * b.m[6] + a.m[10] * b.m[10] +
                            a.m[11] * b.m[14],
                        a.m[8] * b.m[3] + a.m[9] * b.m[7] + a.m[10] * b.m[11] +
                            a.m[11] * b.m[15],
                        a.m[12] * b.m[0] + a.m[13] * b.m[4] + a.m[14] * b.m[8] +
                            a.m[15] * b.m[12],
                        a.m[12] * b.m[1] + a.m[13] * b.m[5] + a.m[14] * b.m[9] +
                            a.m[15] * b.m[13],
                        a.m[12] * b.m[2] + a.m[13] * b.m[6] +
                            a.m[14] * b.m[10] + a.m[15] * b.m[14],
                        a.m[12] * b.m[3] + a.m[13] * b.m[7] +
                            a.m[14] * b.m[11] + a.m[15] * b.m[15]);
    }
    friend inline Matrix3D operator*(const Matrix3D& a, const Matrix3D3& b) {
        return Matrix3D(
            a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6],
            a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7],
            a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8], a.m[3],
            a.m[4] * b.m[0] + a.m[5] * b.m[3] + a.m[6] * b.m[6],
            a.m[4] * b.m[1] + a.m[5] * b.m[4] + a.m[6] * b.m[7],
            a.m[4] * b.m[2] + a.m[5] * b.m[5] + a.m[6] * b.m[8], a.m[7],
            a.m[8] * b.m[0] + a.m[9] * b.m[3] + a.m[10] * b.m[6],
            a.m[8] * b.m[1] + a.m[9] * b.m[4] + a.m[10] * b.m[7],
            a.m[8] * b.m[2] + a.m[9] * b.m[5] + a.m[10] * b.m[8], a.m[11],
            a.m[12] * b.m[0] + a.m[13] * b.m[3] + a.m[14] * b.m[6],
            a.m[12] * b.m[1] + a.m[13] * b.m[4] + a.m[14] * b.m[7],
            a.m[12] * b.m[2] + a.m[13] * b.m[5] + a.m[14] * b.m[8], a.m[15]);
    }
};

#if !NDEBUG
std::string printMatrix(const Matrix3D& m);  // test.cc
#endif

class Renderer3D {
  public:
    static Matrix3D getModelMatrix(Point3D p, Orient3D r, Point3D s);
    void renderModel(SplinterBuffer& buf, Point3D p, Orient3D r, Point3D s,
                     const Model& m);
    void setCamera(Point3D pos, Orient3D rot, Point3D scale);
    Renderer3D();

  private:
    void renderModelFragment(SplinterBuffer& buf, const ModelFragment& f) const;
    void projectVertices(const std::vector<Point3D>& v, const Matrix3D& m);
    Point3D clipPoint(const Vector3D& onScreen,
                      const Vector3D& offScreen) const;
    bool clipLine(const Vector3D& v0, const Vector3D& v1, Point3D& p0,
                  Point3D& p1) const;
    Matrix3D view;
    std::vector<Vector3D> points_;
};
};  // namespace hiemalia

#endif  // M_REND3D_HH
