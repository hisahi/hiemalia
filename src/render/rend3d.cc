/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// rend3d.cc: implementation of Renderer3D

#include "rend3d.hh"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "logger.hh"
#include "math.hh"
#include "sbuf.hh"
#include "shape.hh"

namespace hiemalia {

Matrix3D Matrix3D::translate(const ModelPoint& p) {
    return Matrix3D(1, 0, 0, p.x, 0, 1, 0, p.y, 0, 0, 1, p.z, 0, 0, 0, 1);
}

Matrix3D Matrix3D::scale(coord_t s) {
    return Matrix3D(s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::scaleXYZ(coord_t x, coord_t y, coord_t z) {
    return Matrix3D(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::yaw(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::pitch(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::roll(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::rotate(const Rotation3D& r) {
    Matrix3D m = Matrix3D::identity();
    if (r.roll != 0) m *= Matrix3D::roll(r.roll);
    if (r.pitch != 0) m *= Matrix3D::pitch(r.pitch);
    if (r.yaw != 0) m *= Matrix3D::yaw(r.yaw);
    return m;
}

#if !NDEBUG
std::string printMatrix(const Matrix3D& m) {
    std::ostringstream stream;
    stream << std::fixed << std::showpos << std::setprecision(2)
           << std::setfill(' ') << std::setw(8) << "\n";
    stream << "[ " << m[0] << "  " << m[1] << "  " << m[2] << "  " << m[3]
           << " ]\n";
    stream << "[ " << m[4] << "  " << m[5] << "  " << m[6] << "  " << m[7]
           << " ]\n";
    stream << "[ " << m[8] << "  " << m[9] << "  " << m[10] << "  " << m[11]
           << " ]\n";
    stream << "[ " << m[12] << "  " << m[13] << "  " << m[14] << "  " << m[15]
           << " ]\n";
    return stream.str();
}
#endif

ModelPoint Vector3D::toCartesian() const {
    coord_t wf = 1.0 / w;
    return ModelPoint(x * wf, y * wf, z * wf);
}

Vector3D Vector3D::project(const Matrix3D& m) {
    return Vector3D(x * m[0] + y * m[1] + z * m[2] + w * m[3],
                    x * m[4] + y * m[5] + z * m[6] + w * m[7],
                    x * m[8] + y * m[9] + z * m[10] + w * m[11],
                    x * m[12] + y * m[13] + z * m[14] + w * m[15]);
}

Matrix3D Renderer3D::getModelMatrix(ModelPoint p, Rotation3D r,
                                    coord_t s) const {
    /*Matrix3D wrld = view;*/
    Matrix3D wrld = Matrix3D::translate(p);
    wrld *= Matrix3D::rotate(r);
    wrld *= Matrix3D::scale(s);
    return wrld;
}

Renderer3D::Renderer3D() {
    setCamera(ModelPoint(0, 0, 0), Rotation3D(0, 0, 0), 1);
}

static coord_t computeFOV(coord_t a) { return 1.0 / std::tan(a / 2.0); }

static const coord_t near = 1.0 / 10;
static const coord_t far = 10;
static const coord_t wherever_you_are = far / (far - near);
static const coord_t s_fov = computeFOV(radians<coord_t>(90));

void Renderer3D::setCamera(ModelPoint pos, Rotation3D rot, coord_t scale) {
    // projection
    view = Matrix3D(s_fov, 0, 0, 0, 0, s_fov, 0, 0, 0, 0, wherever_you_are,
                    near * wherever_you_are, 0, 0, 1, 0);
    // view. in reverse order from the model matrix!
    view *= Matrix3D::scale(scale);
    if (rot.yaw != 0) view *= Matrix3D::yaw(-rot.yaw);
    if (rot.pitch != 0) view *= Matrix3D::pitch(-rot.pitch);
    if (rot.roll != 0) view *= Matrix3D::roll(-rot.roll);
    view *= Matrix3D::translate(-pos);
}

void Renderer3D::renderModel(SplinterBuffer& buf, ModelPoint p, Rotation3D r,
                             coord_t s, const Model& m) {
    Matrix3D wrld = view * getModelMatrix(p, r, s);
    points_.clear();
    for (const ModelPoint& p : m.vertices)
        points_.emplace_back<Vector3D>(Vector3D(p).project(wrld));
    for (const ModelFragment& part : m.shapes) renderModelFragment(buf, part);
}

static const coord_t maxDist = 2;

inline static int outcode(const Vector3D& v) {
    return ((v.x > v.w) << 0) | ((v.x < -v.w) << 1) | ((v.y > v.w) << 2) |
           ((v.y < -v.w) << 3) | ((v.z > maxDist) << 4) | ((v.z < near) << 5);
}

ModelPoint Renderer3D::clipPoint(const Vector3D& onScreen,
                                 const Vector3D& offScreen) const {
    coord_t n = unlerp(onScreen.w, near, offScreen.w);
    return Vector3D(lerp(onScreen.x, n, offScreen.x),
                    lerp(onScreen.y, n, offScreen.y),
                    lerp(onScreen.z, n, offScreen.z), near)
        .toCartesian();
}

bool Renderer3D::clipLine(const Vector3D& v0, const Vector3D& v1,
                          ModelPoint& p0, ModelPoint& p1) const {
    int o0 = outcode(v0), o1 = outcode(v1);

    if (!(o0 | o1)) {
        p0 = v0.toCartesian();
        p1 = v1.toCartesian();
        return true;
    }
    if (o0 & o1) return false;

    if (v0.z >= near)
        p0 = v0.toCartesian();
    else
        p1 = clipPoint(v1, v0);

    if (v1.z >= near)
        p1 = v1.toCartesian();
    else
        p1 = clipPoint(v0, v1);
    return true;
}

void Renderer3D::renderModelFragment(SplinterBuffer& buf,
                                     const ModelFragment& f) const {
    bool visible, shape = false;
    Color clr = f.color;
    const Vector3D* v0 = &points_[f.start];
    const Vector3D* v1;
    ModelPoint p0{0, 0, 0}, p1{0, 0, 0};
    for (auto it = f.points.begin(); it != f.points.end(); ++it) {
        v1 = &points_[*it];
        visible = clipLine(*v0, *v1, p0, p1);
        if (visible) {
            if (!shape) {
                buf.push(Splinter{SplinterType::BeginShape, p0.x, p0.y, clr});
                shape = true;
            }
            buf.push(Splinter{SplinterType::Point, p1.x, p1.y, clr});
        } else if (shape) {
            buf.endShape();
            shape = false;
        }
        v0 = v1;
    }

    if (shape) buf.endShape();
}
}  // namespace hiemalia
