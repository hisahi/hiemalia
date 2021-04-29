/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// cbuffer.hh: header file for circular buffer

#ifndef M_CBUFFER_HH
#define M_CBUFFER_HH

#include <array>

#include "math.hh"

namespace hiemalia {

template <typename T, size_t N>
class CircularBuffer {
  public:
    static_assert(N > 0, "cannot have an empty buffer");
    using array_type = std::array<T, 2 * N>;

    CircularBuffer() {}

    void clear() {
        begin_ = 0;
        end_ = 0;
    }

    void push_front(const T& value) {
        if (begin_ == 0) {
            end_ = N + (end_ - begin_);
            begin_ = N - 1;
        } else {
            --begin_;
        }
        place(begin_, value);
        if (end_ - begin_ > N) {
            end_ = begin_ + N;
        }
    }

    void push_back(const T& value) {
        place(end_++, value);
        if (end_ == 2 * N) {
            begin_ = 0;
            end_ = N;
        } else if (end_ - begin_ > N) {
            begin_ = end_ - N;
        }
    }

    const T& operator[](size_t i) const { return data_[begin_ + i]; }
    const T& at(size_t i) const { return data_[begin_ + (i % N)]; }

    void set(size_t i, const T& value) { place(remainder(i, 2 * N), value); }

    size_t size() const noexcept { return end_ - begin_; }
    size_t capacity() const noexcept { return N; }

    typename array_type::iterator begin() { return data_.begin() + begin_; }

    typename array_type::const_iterator begin() const {
        return data_.begin() + begin_;
    }

    typename array_type::iterator end() { return data_.begin() + end_; }

    typename array_type::const_iterator end() const {
        return data_.begin() + end_;
    }

    const T& front() const { return data_[begin_]; }
    const T& back() const { return data_[end_ - 1]; }

  private:
    array_type data_{};
    size_t begin_{0};
    size_t end_{0};

    void place(size_t i, const T& value) {
        if (i >= N) {
            data_[i - N] = value;
            data_[i] = value;
        } else {
            data_[i] = value;
            data_[i + N] = value;
        }
    }
};

}  // namespace hiemalia

#endif  // M_CBUFFER_HH
