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

namespace hiemalia {
static constexpr size_t DEFAULT_BUFFER_SIZE = 4096;

struct Splinter {
    float x1;        // between -1.0 ... 1.0
    float y1;        // between -1.0 ... 1.0
    float x2;        // between -1.0 ... 1.0
    float y2;        // between -1.0 ... 1.0
    uint_fast8_t r;  // 0-255
    uint_fast8_t g;  // 0-255
    uint_fast8_t b;  // 0-255
    uint_fast8_t a;  // 0-255
};

class SplinterBuffer {
   public:
    using container = std::vector<Splinter>;
    using const_iterator = container::const_iterator;

    SplinterBuffer() { _splinters.reserve(DEFAULT_BUFFER_SIZE); }

    ~SplinterBuffer() = default;
    SplinterBuffer(const SplinterBuffer& copy) = delete;
    SplinterBuffer& operator=(const SplinterBuffer& copy) = delete;
    SplinterBuffer(const SplinterBuffer&& move)
        : _splinters(std::move(move._splinters)) {}
    SplinterBuffer& operator=(const SplinterBuffer&& move) {
        _splinters = std::move(move._splinters);
        return *this;
    }

    void clear() noexcept { _splinters.clear(); }
    void push(Splinter splinter) { _splinters.push_back(splinter); }
    const_iterator begin() const noexcept { return _splinters.begin(); }
    const_iterator end() const noexcept { return _splinters.end(); }

   private:
    container _splinters;
};
};  // namespace hiemalia

#endif  // M_VBUF_HH
