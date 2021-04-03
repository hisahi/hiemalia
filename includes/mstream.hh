/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// mstream.hh: header file for memory streams

#ifndef M_MSTREAM_HH
#define M_MSTREAM_HH

#include <stdio.h>

#include <iostream>
#include <iterator>
#include <vector>

namespace hiemalia {

namespace {
constexpr std::streamsize buffer_block_size = BUFSIZ;
constexpr size_t alloc_size = BUFSIZ;

static constexpr size_t round_alloc_size(size_t n) {
    return (n + alloc_size - 1) / alloc_size * alloc_size;
}
}  // namespace

template <typename Allocator = std::allocator<char>>
class memorybuf : public std::streambuf {
    using traits = std::char_traits<char_type>;

   public:
    inline explicit memorybuf(
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
        : alloc_(), n_(0), p_(nullptr), mode_(which) {
        setg(p_, which & std::ios_base::in ? p_ : nullptr, p_);
        setp(which & std::ios_base::out ? p_ : nullptr, p_);
    }
    inline explicit memorybuf(
        const std::vector<char_type>& v,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
        : alloc_(),
          n_(round_alloc_size(v.size())),
          p_(alloc_.allocate(n_)),
          mode_(which) {
        setg(p_, which & std::ios_base::in ? p_ : nullptr, p_ + v.size());
        setp(which & std::ios_base::out ? p_ + v.size() : nullptr, p_ + n_);
    }
    inline memorybuf(const char_type* p, size_t n,
                     std::ios_base::openmode which = std::ios_base::in |
                                                     std::ios_base::out)
        : alloc_(),
          n_(round_alloc_size(n)),
          p_(alloc_.allocate(n_)),
          mode_(which) {
        std::copy(p, p + n, p_);
        setg(p_, which & std::ios_base::in ? p_ : nullptr, p_ + n);
        setp(which & std::ios_base::out ? p_ + n : nullptr, p_ + n_);
    }
    template <typename InputIt>
    inline memorybuf(InputIt begin, InputIt end,
                     std::ios_base::openmode which = std::ios_base::in |
                                                     std::ios_base::out)
        : alloc_(), mode_(which) {
        size_t n = std::distance(begin, end);
        n_ = round_alloc_size(n);
        p_ = alloc_.allocate(n_);
        std::copy(begin(), end(), p_);
        setg(p_, which & std::ios_base::in ? p_ : nullptr, p_ + n);
        setp(which & std::ios_base::out ? p_ + n : nullptr, p_ + n_);
    }
    inline explicit memorybuf(
        std::istream& s,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
        : alloc_(), n_(0), p_(nullptr), mode_(which) {
        auto state = s.exceptions();
        std::streamsize n = 0, q;
        s.exceptions(state & ~std::ios_base::eofbit);
        auto& buf = *s.rdbuf();
        do {
            grow_(buffer_block_size);
            n += q = buf.sgetn(p_ + n, buffer_block_size);
        } while (q == buffer_block_size);
        s.exceptions(state);
        setg(p_, which & std::ios_base::in ? p_ : nullptr, p_ + n);
        setp(which & std::ios_base::out ? p_ + n : nullptr, p_ + n_);
    }

    inline memorybuf(const memorybuf& copy)
        : std::streambuf(copy),
          alloc_(copy.alloc_),
          n_(copy.n_),
          p_(new char[n_]),
          mode_(copy.mode_) {
        std::copy(copy.p_, copy.p_ + n_, p_);
        ptrdiff_t off = p_ - copy.p_;
        setg(p_, mode_ & std::ios_base::in ? copy.gptr() + off : nullptr,
             copy.egptr() + off);
        setp(mode_ & std::ios_base::out ? copy.pptr() + off : nullptr, p_ + n_);
    }

    inline memorybuf(memorybuf&& move) noexcept
        : std::streambuf(std::move(move)),
          alloc_(std::move(move.alloc_)),
          n_(move.n_),
          p_(std::exchange(move.p_, nullptr)),
          mode_(move.mode_) {
        setg(p_, move.gptr(), move.egptr());
        setp(move.pptr(), move.epptr());
        move.setg(nullptr, nullptr, nullptr);
        move.setp(nullptr, nullptr);
    }

    inline memorybuf& operator=(const memorybuf& copy) {
        resize_(copy.n_);
        alloc_ = copy.alloc_;
        mode_ = copy.mode_;
        std::copy(copy.p_, copy.p_ + n_, p_);
        ptrdiff_t off = p_ - copy.p_;
        setg(p_, mode_ & std::ios_base::in ? copy.gptr() + off : nullptr,
             copy.egptr() + off);
        setp(mode_ & std::ios_base::out ? copy.pptr() + off : nullptr, p_ + n_);
        return *this;
    }

    inline memorybuf& operator=(memorybuf&& move) noexcept {
        p_ = std::exchange(move.p_, nullptr);
        n_ = move.n_;
        alloc_ = std::move(move.alloc_);
        mode_ = move.mode_;
        setg(p_, move.gptr(), move.egptr());
        setp(move.pptr(), move.epptr());
        move.setg(nullptr, nullptr, nullptr);
        move.setp(nullptr, nullptr);
        return *this;
    }

    ~memorybuf() noexcept {
        if (p_) alloc_.deallocate(p_, n_);
    }

    inline void flush() {
        ptrdiff_t offset = gptr() ? gptr() - eback() : pptr() - pbase();
        if (offset > 0) {
            std::copy(p_ + offset, p_ + n_, p_);
            resize_(n_ - offset);
            if (mode_ & std::ios_base::in)
                setg(p_, gptr() - offset, egptr() - offset);
            if (mode_ & std::ios_base::out) setp(pptr() - offset, p_ + n_);
        }
    }

    inline void clear() {
        resize_(0);
        setg(nullptr, nullptr, nullptr);
        setp(nullptr, nullptr);
    }

   protected:
    inline int_type underflow() {
        if (!(mode_ & std::ios_base::in)) return traits::eof();
        if ((mode_ & std::ios_base::out) && pptr() > egptr())
            setg(eback(), gptr(), pptr());
        if (gptr() < egptr()) {
            gbump(1);
            return traits::to_int_type(gptr()[-1]);
        } else
            return traits::eof();
    }

    inline int_type pbackfail(int_type c = traits::eof()) {
        if (traits::eq_int_type(c, traits::eof())) {
            if (!gptr() || eback() == gptr()) return traits::eof();
            gbump(-1);
            return traits::not_eof(c);
        } else {
            if (!gptr() || eback() == gptr()) return traits::eof();
            if (!(mode_ & std::ios_base::out)) return traits::eof();
            gbump(-1);
            if (!traits::eq(traits::to_char_type(c), *gptr())) {
                *gptr() = c;
            }
            return c;
        }
    }

    inline int_type overflow(int_type c = traits::eof()) {
        if (traits::eq_int_type(c, traits::eof())) return traits::not_eof(0);
        if (!(mode_ & std::ios_base::out)) return traits::eof();
        char* old_ptr = p_;
        grow_(alloc_size);
        ptrdiff_t off = p_ - old_ptr;
        if (mode_ & std::ios_base::in) setg(p_, gptr() + off, egptr() + off);
        setp(pptr() + off, p_ + n_);
        *pptr() = traits::to_char_type(c);
        pbump(1);
        return c;
    }

    inline pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                            std::ios_base::openmode which =
                                std::ios_base::in | std::ios_base::out) {
        off_type n = off_type(-1);
        switch (dir) {
            case std::ios_base::beg:
                n = 0;
                break;
            case std::ios_base::cur:
                n = gptr() - eback();
                break;
            case std::ios_base::end:
                n = egptr() - eback();
                break;
            default:
                return pos_type(off_type(-1));
        }
        n += off;
        if (n < 0 || n >= egptr() - eback() ||
            (n != 0 && (((which & std::ios_base::in) && gptr() == nullptr) ||
                        ((which & std::ios_base::out) && pptr() == nullptr))))
            return pos_type(off_type(-1));
        if (which & std::ios_base::in) setg(eback(), p_ + n, egptr());
        if (which & std::ios_base::out) setp(p_ + n, epptr());
        return pos_type(n);
    }

    inline pos_type seekpos(
        pos_type sp, std::ios_base::openmode which = std::ios_base::in |
                                                     std::ios_base::out) {
        return seekoff(off_type(sp), std::ios_base::beg, which);
    }

    inline std::streamsize xsgetn(char_type* s, std::streamsize count) {
        if (!(mode_ & std::ios_base::in)) return 0;
        if (pptr() > egptr()) setg(eback(), gptr(), pptr());
        std::streamsize n = std::min<std::streamsize>(count, egptr() - gptr());
        std::copy(gptr(), gptr() + n, s);
        gbump(static_cast<int>(n));
        return n;
    }

    inline std::streamsize xsputn(const char_type* s, std::streamsize count) {
        if (!(mode_ & std::ios_base::out)) return 0;
        char* old_ptr = p_;
        grow_(static_cast<size_t>(count));
        if (p_ != old_ptr) {
            ptrdiff_t off = p_ - old_ptr;
            setg(p_, gptr() + off, egptr() + off);
            setp(pptr() + off, p_ + n_);
        }
        std::copy(s, s + count, pptr());
        pbump(static_cast<int>(count));
        return count;
    }

    inline void realloc_(size_t n) {
        char* new_p_;
        if (n == 0) {
            new_p_ = nullptr;
        } else {
            new_p_ = alloc_.allocate(n);
            if (p_) std::copy(p_, p_ + n_, new_p_);
        }
        if (p_) alloc_.deallocate(p_, n_);
        p_ = new_p_;
        n_ = n;
    }

    inline void resize_(size_t n) {
        n = round_alloc_size(n);
        if (n_ != n) realloc_(n);
    }

    inline void grow_(size_t n) { resize_(n_ + n); }

   private:
    Allocator alloc_;
    size_t n_;
    char* p_;
    std::ios_base::openmode mode_;

    friend class imemorystream;
    friend class omemorystream;
    friend class memorystream;
};

class imemorystream : public std::istream {
   public:
    inline explicit imemorystream(const std::vector<char>& v)
        : std::istream(&vbuf_), vbuf_(v, std::ios_base::in) {}
    inline explicit imemorystream(std::istream& scopy)
        : std::istream(&vbuf_), vbuf_(scopy, std::ios_base::in) {}
    inline imemorystream(const imemorystream& copy)
        : std::basic_ios<char_type>(&vbuf_),
          std::istream(&vbuf_),
          vbuf_(copy.vbuf_) {}
    inline imemorystream(imemorystream&& move) noexcept
        : std::istream(std::move(move)), vbuf_(std::move(move.vbuf_)) {}
    inline char* ibegin() const noexcept { return vbuf_.gptr(); }
    inline char* iend() const noexcept { return vbuf_.egptr(); }
    inline std::vector<char_type> idatacopy() const noexcept {
        return std::vector<char_type>(ibegin(), iend());
    }
    inline size_t isize() { return iend() - ibegin(); }
    inline void clear() { vbuf_.clear(); }

   private:
    memorybuf<> vbuf_;
};

class omemorystream : public std::ostream {
   public:
    inline omemorystream() : std::ostream(&vbuf_), vbuf_(std::ios_base::out) {}
    inline omemorystream(const omemorystream& copy)
        : std::basic_ios<char_type>(&vbuf_),
          std::ostream(&vbuf_),
          vbuf_(copy.vbuf_) {}
    inline omemorystream(omemorystream&& move) noexcept
        : std::ostream(std::move(move)), vbuf_(std::move(move.vbuf_)) {}
    inline char* obegin() const noexcept { return vbuf_.pbase(); }
    inline char* oend() const noexcept { return vbuf_.pptr(); }
    inline std::vector<char_type> odatacopy() const noexcept {
        return std::vector<char_type>(obegin(), oend());
    }
    inline size_t osize() { return oend() - obegin(); }
    inline void flush() { vbuf_.flush(); }
    inline void clear() { vbuf_.clear(); }
    inline size_t copy_to(std::ostream& s) {
        const char_type* p = obegin();
        const char_type* e = oend();
        size_t t = 0, n;
        while (p < e && !s.fail() && !s.bad()) {
            n = std::min<size_t>(buffer_block_size, e - p);
            s.write(p, n);
            p += n;
            t += n;
        }
        vbuf_.flush();
        return t;
    }

   private:
    memorybuf<> vbuf_;
};

class memorystream : public std::iostream {
   public:
    inline explicit memorystream(const std::vector<char>& v)
        : std::iostream(&vbuf_),
          vbuf_(v, std::ios_base::in | std::ios_base::out) {}
    inline explicit memorystream(std::istream& scopy)
        : std::iostream(&vbuf_),
          vbuf_(scopy, std::ios_base::in | std::ios_base::out) {}
    inline memorystream(const memorystream& copy)
        : std::basic_ios<char_type>(&vbuf_),
          std::iostream(&vbuf_),
          vbuf_(copy.vbuf_) {}
    inline memorystream(memorystream&& move) noexcept
        : std::iostream(std::move(move)), vbuf_(std::move(move.vbuf_)) {}
    inline char* ibegin() const noexcept { return vbuf_.gptr(); }
    inline char* iend() const noexcept { return vbuf_.egptr(); }
    inline std::vector<char_type> idatacopy() const noexcept {
        return std::vector<char_type>(ibegin(), iend());
    }
    inline size_t isize() { return iend() - ibegin(); }
    inline char* obegin() const noexcept { return vbuf_.gptr(); }
    inline char* oend() const noexcept { return vbuf_.pptr(); }
    inline std::vector<char_type> odatacopy() const noexcept {
        return std::vector<char_type>(obegin(), oend());
    }
    inline size_t osize() { return oend() - obegin(); }
    inline void flush() { vbuf_.flush(); }
    inline void clear() { vbuf_.clear(); }
    inline size_t copy_to(std::ostream& s) {
        const char_type* p = obegin();
        const char_type* e = oend();
        size_t t = 0, n;
        while (p < e && !s.fail() && !s.bad()) {
            n = std::min<size_t>(buffer_block_size, e - p);
            s.write(p, n);
            p += n;
            t += n;
        }
        vbuf_.gbump(t);
        vbuf_.flush();
        return t;
    }

   private:
    memorybuf<> vbuf_;
};

};  // namespace hiemalia

#endif  // M_MSTREAM_HH
