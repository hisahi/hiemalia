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

Matrix3D Matrix3D::translate(const Point3D& p) {
    return Matrix3D(1, 0, 0, p.x, 0, 1, 0, p.y, 0, 0, 1, p.z, 0, 0, 0, 1);
}

Matrix3D3 Matrix3D3::scale(const Point3D& s) {
    return Matrix3D3(s.x, 0, 0, 0, s.y, 0, 0, 0, s.z);
}

Matrix3D3 Matrix3D3::scale(coord_t s) {
    return Matrix3D3(s, 0, 0, 0, s, 0, 0, 0, s);
}

Matrix3D3 Matrix3D3::yaw(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D3(c, 0, s, 0, 1, 0, -s, 0, c);
}

Matrix3D3 Matrix3D3::pitch(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D3(1, 0, 0, 0, c, s, 0, -s, c);
}

Matrix3D3 Matrix3D3::roll(coord_t theta) {
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D3(c, -s, 0, s, c, 0, 0, 0, 1);
}

Matrix3D3 Matrix3D3::rotate(const Orient3D& r) {
    Matrix3D3 m = r.yaw != 0 ? Matrix3D3::yaw(r.yaw) : Matrix3D3::identity();
    if (r.pitch != 0) m *= Matrix3D3::pitch(r.pitch);
    if (r.roll != 0) m *= Matrix3D3::roll(r.roll);
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

Point3D Vector3D::toCartesian() const {
    coord_t wf = 1.0 / w;
    dynamic_assert(wf > 0, "negative w -- unexpected degenerate case");
    return Point3D(x * wf, y * wf, z * wf);
}

Vector3D Matrix3D3::project(const Vector3D& v) const {
    coord_t x = v.x, y = v.y, z = v.z, w = v.w;
    return Vector3D(x * m[0] + y * m[1] + z * m[2],
                    x * m[3] + y * m[4] + z * m[5],
                    x * m[6] + y * m[7] + z * m[8], w);
}

Point3D Matrix3D3::project(const Point3D& p) const {
    return project(Vector3D(p)).toCartesian();
}

Vector3D Matrix3D::project(const Vector3D& v) const {
    coord_t x = v.x, y = v.y, z = v.z, w = v.w;
    return Vector3D(x * m[0] + y * m[1] + z * m[2] + w * m[3],
                    x * m[4] + y * m[5] + z * m[6] + w * m[7],
                    x * m[8] + y * m[9] + z * m[10] + w * m[11],
                    x * m[12] + y * m[13] + z * m[14] + w * m[15]);
}

Point3D Matrix3D::project(const Point3D& p) const {
    return project(Vector3D(p)).toCartesian();
}

Matrix3D Renderer3D::getModelMatrix(Point3D p, Orient3D r, Point3D s) {
    Matrix3D wrld = Matrix3D::translate(p);
    wrld *= Matrix3D3::rotate(r);
    wrld *= Matrix3D3::scale(s);
    return wrld;
}

coord_t Orient3D::offBy(const Orient3D& other) const noexcept {
    return std::max({angleDifferenceAbs(yaw, other.yaw),
                     angleDifferenceAbs(pitch, other.pitch),
                     angleDifferenceAbs(roll, other.roll)});
}

Point3D Orient3D::rotate(const Point3D& vector, coord_t scale) const noexcept {
    return Matrix3D3::rotate(*this).project(vector) * scale;
}

Point3D Orient3D::direction(coord_t scale /* = 1 */) const noexcept {
    return rotate(Point3D(0, 0, scale));
}

Orient3D Orient3D::toPolar(const Point3D& p, coord_t roll) noexcept {
    coord_t yaw = atan2(p.x, p.z);
    coord_t pitch = atan2(p.y, hypot(p.x, p.z));
    return Orient3D(yaw, pitch, roll);
}

Orient3D Orient3D::tendTo(const Orient3D& target, coord_t rate) const noexcept {
    return Orient3D(angleApproach(yaw, target.yaw, rate),
                    angleApproach(pitch, target.pitch, rate),
                    angleApproach(roll, target.roll, rate));
}

Renderer3D::Renderer3D() {
    setCamera(Point3D(0, 0, 0), Orient3D(0, 0, 0), Point3D(1, 1, 1));
}

static coord_t computeFOV(coord_t a) { return 1.0 / tan(a / 2.0); }

static const coord_t near = 1.0 / 256;
static const coord_t far = 8;
static const coord_t wherever_you_are = far / (far - near);
static const coord_t s_fov = computeFOV(radians<coord_t>(90));

void Renderer3D::setCamera(Point3D pos, Orient3D rot, Point3D scale) {
    // projection
    view = Matrix3D(s_fov, 0, 0, 0, 0, s_fov, 0, 0, 0, 0, wherever_you_are,
                    near * wherever_you_are, 0, 0, 1, 0);
    // view. in reverse order from the model matrix!
    view *= Matrix3D3::scale(scale);
    if (rot.roll != 0) view *= Matrix3D3::roll(-rot.roll);
    if (rot.pitch != 0) view *= Matrix3D3::pitch(-rot.pitch);
    if (rot.yaw != 0) view *= Matrix3D3::yaw(-rot.yaw);
    view *= Matrix3D::translate(-pos);
}

void Renderer3D::renderModel(SplinterBuffer& buf, Point3D p, Orient3D r,
                             Point3D s, const Model& m) {
    Matrix3D wrld = view * getModelMatrix(p, r, s);
    points_.clear();
    for (const Point3D& p : m.vertices)
        points_.emplace_back<Vector3D>(wrld.project(Vector3D(p)));
    for (const ModelFragment& part : m.shapes) renderModelFragment(buf, part);
}

inline static int outcode(const Vector3D& v) {
    return static_cast<int>((v.x > v.w) << 0) |
           static_cast<int>((v.x < -v.w) << 1) |
           static_cast<int>((v.y > v.w) << 2) |
           static_cast<int>((v.y < -v.w) << 3) |
           static_cast<int>((v.z > viewDistance) << 4) |
           static_cast<int>((v.w < near) << 5);
}

Point3D Renderer3D::clipPoint(const Vector3D& onScreen,
                              const Vector3D& offScreen) {
    coord_t n = unlerp(onScreen.w, near, offScreen.w);
    return Vector3D(lerp(onScreen.x, n, offScreen.x),
                    lerp(onScreen.y, n, offScreen.y),
                    lerp(onScreen.z, n, offScreen.z), near)
        .toCartesian();
}

bool Renderer3D::clipLine(const Vector3D& v0, const Vector3D& v1, Point3D& p0,
                          Point3D& p1) const {
    int o0 = outcode(v0), o1 = outcode(v1);

    if (!(o0 | o1)) {
        p0 = v0.toCartesian();
        p1 = v1.toCartesian();
        return true;
    }
    if (o0 & o1) return false;

    p0 = v0.w >= near ? v0.toCartesian() : clipPoint(v1, v0);
    p1 = v1.w >= near ? v1.toCartesian() : clipPoint(v0, v1);
    return true;
}

void Renderer3D::renderModelFragment(SplinterBuffer& buf,
                                     const ModelFragment& f) const {
    bool visible, shape = false;
    Color clr = f.color;
    const Vector3D* v0 = &points_[f.start];
    const Vector3D* v1;
    Point3D p0{0, 0, 0}, p1{0, 0, 0};
    for (size_t p : f.points) {
        v1 = &points_[p];
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
