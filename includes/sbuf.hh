/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sbuf.hh: header file for splinter buffer

#ifndef M_VBUF_HH
#define M_VBUF_HH

#include <cstdint>
#include <vector>

#include "defs.hh"

namespace hiemalia {
static constexpr size_t DEFAULT_BUFFER_SIZE = 128;
static constexpr size_t DEFAULT_SCREEN_BUFFER_SIZE = 8192;

enum class SplinterType {
    BeginShape,
    Point,
    EndShapePoint,
    BeginClipCenter,
    EndClip
};

struct Color {
    uint8_t r;  // 0-255
    uint8_t g;  // 0-255
    uint8_t b;  // 0-255
    uint8_t a;  // 0-255

    // video.cc
    static Color fromHSVA(float h, float s, float v, float a);
};

struct Splinter {
    SplinterType type;
    coord_t x;  // between -1.0 (left) ... 1.0 (right)
    coord_t y;  // between -1.0 (top)  ... 1.0 (bottom)
    union {
        int sentinel{0};
        Color color;
    };

    Splinter(SplinterType type, coord_t x, coord_t y)
        : type(type), x(x), y(y) {}
    Splinter(SplinterType type, coord_t x, coord_t y, Color color)
        : type(type), x(x), y(y), color(color) {}
};

class SplinterBuffer {
  public:
    using container = std::vector<Splinter>;
    using const_iterator = container::const_iterator;

    explicit SplinterBuffer(size_t sz) { _splinters.reserve(sz); }
    SplinterBuffer() : SplinterBuffer(DEFAULT_BUFFER_SIZE) {}

    ~SplinterBuffer() = default;
    SplinterBuffer(const SplinterBuffer& copy) = delete;
    SplinterBuffer& operator=(const SplinterBuffer& copy) = delete;
    SplinterBuffer(SplinterBuffer&& move) noexcept
        : _splinters(std::move(move._splinters)) {}
    SplinterBuffer& operator=(SplinterBuffer&& move) noexcept {
        _splinters = std::move(move._splinters);
        return *this;
    }

    inline void clear() noexcept { _splinters.clear(); }
    inline void push(Splinter splinter) { _splinters.push_back(splinter); }
    inline void append(const SplinterBuffer& sbuf) {
        _splinters.insert(_splinters.end(), sbuf._splinters.begin(),
                          sbuf._splinters.end());
    }
    inline void endShape() {
        dynamic_assert(_splinters.size() > 0, "cannot end non-shape");
        dynamic_assert(_splinters.back().type == SplinterType::Point,
                       "cannot end non-shape");
        _splinters.back().type = SplinterType::EndShapePoint;
    }

    const_iterator begin() const noexcept { return _splinters.begin(); }
    const_iterator end() const noexcept { return _splinters.end(); }
    size_t size() const noexcept { return _splinters.size(); }

  private:
    container _splinters;
};
};  // namespace hiemalia

#endif  // M_VBUF_HH
