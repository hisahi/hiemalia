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
    if (r.roll != 0) m *= Matrix3D::roll(-r.roll);
    if (r.pitch != 0) m *= Matrix3D::pitch(-r.pitch);
    if (r.yaw != 0) m *= Matrix3D::yaw(-r.yaw);
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
    setCamera(ModelPoint(0, 0, 0), Rotation3D(0, 0, 0));
}

static coord_t computeFOV(coord_t a) { return 1.0 / std::tan(a / 2.0); }

static constexpr coord_t near = 1.0 / 10;
static constexpr coord_t far = 10;
static const coord_t wherever_you_are = far / (far - near);
static const coord_t s_fov = computeFOV(radians<coord_t>(120));

void Renderer3D::setCamera(ModelPoint pos, Rotation3D rot) {
    // projection
    view = Matrix3D(s_fov, 0, 0, 0, 0, s_fov, 0, 0, 0, 0, wherever_you_are,
                    -near * wherever_you_are, 0, 0, 1, 0);
    // view. in reverse order from the model matrix!
    // view *= Matrix3D::scale(1.0);
    view *= Matrix3D::yaw(rot.yaw);
    view *= Matrix3D::pitch(rot.pitch);
    view *= Matrix3D::roll(rot.roll);
    view *= Matrix3D::translate(-pos);
}

void Renderer3D::renderModel(SplinterBuffer& buf, ModelPoint p, Rotation3D r,
                             coord_t s, const Model& m) {
    Matrix3D wrld = view * getModelMatrix(p, r, s);
    points_.clear();
    for (const ModelPoint& p : m.vertices)
        points_.emplace_back<ScreenPoint>(Vector3D(p).project(wrld));
    for (const ModelFragment& part : m.shapes) renderModelFragment(buf, part);
}

ModelPoint Renderer3D::clip(const ScreenPoint& onScreen,
                            const ScreenPoint& offScreen) const {
    const Vector3D& a = onScreen.v;
    const Vector3D& b = offScreen.v;
    coord_t n = (b.w - near) / (a.w - b.w);
    return Vector3D(lerp(b.x, n, a.x), lerp(b.y, n, a.y), lerp(b.z, n, a.z),
                    near)
        .toCartesian();
}

void Renderer3D::renderModelFragment(SplinterBuffer& buf,
                                     const ModelFragment& f) const {
    bool shape = false;
    Color clr = f.color;
    const ScreenPoint* v0 = &points_[f.start];
    const ScreenPoint* v1;
    for (auto it = f.points.begin(); it != f.points.end(); ++it) {
        v1 = &points_[*it];
        if (v1->onScreen || (v0->onScreen && shape)) {
            if (!shape) {
                ModelPoint p0 =
                    v0->inFront ? v0->toCartesian() : clip(*v1, *v0);
                buf.push(Splinter{SplinterType::BeginShape, p0.x, p0.y, clr});
                shape = true;
            }

            ModelPoint p1 = v1->inFront ? v1->toCartesian() : clip(*v0, *v1);
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
