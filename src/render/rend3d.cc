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

Matrix3D Matrix3D::translate(ModelPoint p) {
    return Matrix3D(1, 0, 0, p.x, 0, 1, 0, p.y, 0, 0, 1, p.z, 0, 0, 0, 1);
}

Matrix3D Matrix3D::scale(coord_t s) {
    return Matrix3D(s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::yaw(coord_t theta) {
    if (!theta) return Matrix3D::identity();
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::pitch(coord_t theta) {
    if (!theta) return Matrix3D::identity();
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
}

Matrix3D Matrix3D::roll(coord_t theta) {
    if (!theta) return Matrix3D::identity();
    coord_t s = sin(theta), c = cos(theta);
    return Matrix3D(c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
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

Vector3D Vector3D::project(const Matrix3D& m) {
    return Vector3D(x * m[0] + y * m[1] + z * m[2] + w * m[3],
                    x * m[4] + y * m[5] + z * m[6] + w * m[7],
                    x * m[8] + y * m[9] + z * m[10] + w * m[11],
                    x * m[12] + y * m[13] + z * m[14] + w * m[15]);
}

Matrix3D Renderer3D::getModelMatrix(ModelPoint p, Rotation3D r,
                                    coord_t s) const {
    Matrix3D wrld = view;
    wrld *= Matrix3D::translate(p);
    wrld *= Matrix3D::roll(-r.roll);
    wrld *= Matrix3D::pitch(-r.pitch);
    wrld *= Matrix3D::yaw(-r.yaw);
    wrld *= Matrix3D::scale(s);
    return wrld;
}

void Renderer3D::renderModel(SplinterBuffer& buf, ModelPoint p, Rotation3D r,
                             coord_t s, const Model& m) const {
    Matrix3D wrld = getModelMatrix(p, r, s);
    for (const ModelFragment& part : m.parts)
        renderModelFragment(buf, wrld, part);
}

Renderer3D::Renderer3D() {
    setCamera(ModelPoint(0, 0, -4), Rotation3D(0, 0, 0));
}

static coord_t computeFOV(coord_t a) { return 1.0 / std::tan(a / 2); }

static const coord_t s = computeFOV(radians(90));
static const coord_t near = 0;
static const coord_t far = 12;
static const coord_t wherever_you_are = far / (near - far);

void Renderer3D::setCamera(ModelPoint pos, Rotation3D rot) {
    view = Matrix3D(s, 0, 0, 0, 0, s, 0, 0, 0, 0, wherever_you_are, -1, 0, 0,
                    near * wherever_you_are, 0);
    view *= Matrix3D::yaw(rot.yaw);
    view *= Matrix3D::pitch(rot.pitch);
    view *= Matrix3D::roll(rot.roll);
    view *= Matrix3D::translate(-pos);
}

void Renderer3D::renderModelFragment(SplinterBuffer& buf, const Matrix3D& wrld,
                                     const ModelFragment& f) const {
    bool shape = false;
    bool prevOnScreen = false;
    Color clr = f.color;
    ModelPoint p1(0, 0, 0);
    for (auto it = f.points.begin(); it != f.points.end(); ++it) {
        Vector3D v = Vector3D(*it).project(wrld);
        bool onScreen = v.x >= -1 && v.x <= 1 && v.y >= -1 && v.y <= 1 &&
                        v.z >= 0 && v.z <= 1;
        if (onScreen || (prevOnScreen && shape)) {
            ModelPoint p = v.toCartesian();
            if (!shape) {
                buf.push(Splinter{SplinterType::BeginShape, p1.x, p1.y, clr});
                shape = true;
            }
            buf.push(Splinter{SplinterType::Point, p.x, p.y, clr});
            p1 = p;
        } else if (shape) {
            buf.endShape();
            shape = false;
        }
        prevOnScreen = onScreen;
    }

    if (shape) buf.endShape();
}
}  // namespace hiemalia
