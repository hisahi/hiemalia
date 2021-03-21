/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// lvector.hh: limited capacity vector

#ifndef M_LVECTOR_HH
#define M_LVECTOR_HH

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace hiemalia {

template <typename T>
class LimitedVectorIterator;

template <typename T, size_t N, typename Allocator = std::allocator<T>>
class LimitedVector {
   public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using rvalue_reference = T&&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = LimitedVectorIterator<T>;
    using const_iterator = LimitedVectorIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using allocator_type = Allocator;

    explicit LimitedVector(allocator_type alloc)
        : count_(0), alloc_(alloc), data_(alloc_.allocate(N)) {}
    LimitedVector() : LimitedVector(allocator_type()) {}
    explicit LimitedVector(size_type count, const T& value = T(),
                           const allocator_type& alloc = allocator_type())
        : LimitedVector(alloc) {
        size_type i;
        for (i = 0; i < count; ++i) {
            if (i >= N) out_of_capacity();
            new (&data_[i]) T(value);
        }
        count_ = i;
    }
    template <class InputIt>
    explicit LimitedVector(InputIt first, InputIt last,
                           const allocator_type& alloc = allocator_type())
        : LimitedVector(alloc) {
        size_type i;
        while (first < last) {
            if (i >= N) out_of_capacity();
            new (&data_[i]) T(*first++);
        }
        count_ = i;
    }

    LimitedVector(const LimitedVector& copy)
        : count_(copy.count), alloc_(copy.alloc_), data_(alloc_.allocate(N)) {
        for (size_type i = 0; i < count_; ++i) {
            new (&data_[i]) T(copy.data_[i]);
        }
    }

    LimitedVector(LimitedVector&& move)
        : count_(move.count), alloc_(std::move(move.alloc_)) {
        std::swap(data_, move.data_);
    }

    LimitedVector& operator=(const LimitedVector& copy) {
        if (&copy != this) {
            size_type i;

            for (i = 0; i < std::min(count_, copy.count_); ++i) {
                data_[i] = copy.data_[i];
            }

            if (copy.count_ > count_) {
                for (i = count_; i < copy.count_; ++i) {
                    new (&data_[i]) T(copy.data_[i]);
                }
            } else if (copy.count_ < count_) {
                for (i = copy.count_; i < count_; ++i) {
                    data_[i].~T();
                }
            }

            alloc_ = copy.alloc_;
            count_ = copy.count_;
        }
        return *this;
    }

    LimitedVector& operator=(LimitedVector&& move) {
        if (&move != this) {
            for (size_t i = 0; i < count_; ++i) {
                data_[i].~T();
            }
            alloc_.deallocate(data_, N);

            alloc_ = move.alloc_;
            count_ = move.count_;
            data_ = move.data_;
            move.data_ = nullptr;
        }
        return *this;
    }

    ~LimitedVector() {
        if (data_ != nullptr) {
            for (size_t i = 0; i < count_; ++i) {
                data_[i].~T();
            }
            alloc_.deallocate(data_, N);
        }
    }

    void assign(size_type count, const_reference value) {
        size_type i;

        for (i = 0; i < std::min(count_, count); ++i) {
            data_[i] = value;
        }

        if (count > count_) {
            for (i = count_; i < count; ++i) {
                new (&data_[i]) T(value);
            }
        } else if (count < count_) {
            for (i = count; i < count_; ++i) {
                data_[i].~T();
            }
        }

        count_ = count;
    }

    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        InputIt it = first;
        size_type i;

        for (i = 0; i < count_ && it != last; ++i) {
            data_[i++] = *it++;
        }

        if (i < count_) {
            for (; i < count_; ++i) {
                data_[i].~T();
            }
        } else if (first != last) {
            while (it != last) {
                if (i >= N) out_of_capacity();
                new (&data_[i++]) T(*it++);
            }
        }

        count_ = i;
    }

    void assign(std::initializer_list<T> list) {
        assign(list.begin(), list.end());
    }

    allocator_type get_allocator() const { return alloc_; }

    reference at(size_type index) {
        if (index >= count_) throw std::out_of_range("index");
        return *this[index];
    }

    reference operator[](size_type index) { return data[index]; }

    reference front() { return data[0]; }

    reference back() { return data[count_ - 1]; }

    pointer data() { return data_; }

    iterator begin() noexcept { return get_iterator_(0); }

    const_iterator begin() const noexcept { return get_const_iterator_(0); }

    const_iterator cbegin() const noexcept { return get_const_iterator_(0); }

    iterator end() noexcept { return get_iterator_(count_); }

    const_iterator end() const noexcept { return get_const_iterator_(count_); }

    const_iterator cend() const noexcept { return get_const_iterator_(count_); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(begin());
    }

    reverse_iterator rend() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(end());
    }

    bool empty() const noexcept { return count_ == 0; }

    size_type size() const noexcept { return count_; }

    size_type max_size() const noexcept { return N; }

    size_type capacity() const noexcept { return N; }

    void clear() noexcept {
        for (size_type i = 0; i < count_; ++i) data_[i].~T();
        count_ = 0;
    }

    iterator insert(const_iterator pos, const_reference value) {
        size_type i = get_index_(pos);
        move_forward_(i, 1);
        new (&data_[i]) T(value);
        return get_iterator_(i);
    }

    iterator insert(const_iterator pos, size_t count, const_reference value) {
        size_type i = get_index_(pos), j, s = i;
        move_forward_(i, count);
        for (j = 0; j < count; ++j) new (&data_[i++]) T(value);
        return get_iterator_(s);
    }

    template <class InputIt>
    iterator insert(const_iterator pos, InputIt begin, InputIt end) {
        size_type i = get_index_(pos), s = i;
        InputIt it = begin;
        move_forward_(i, end - begin);
        while (it != end) new (&data_[i++]) T(*it++);
        return get_iterator_(s);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> list) {
        return insert(pos, list.begin(), list.end());
    }

    template <typename... Ts>
    iterator emplace(const_iterator pos, Ts&&... args) {
        size_type i = get_index_(pos);
        move_forward_(i, 1);
        new (&data_[i]) T(std::forward<Ts>(args)...);
        return get_iterator_(i);
    }

    iterator erase(const_iterator pos) {
        size_type i = get_index_(pos);
        move_back_(i, 1);
        return get_iterator_(i);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_type i = get_index_(first), n = last - first;
        move_back_(i, n);
        return get_iterator_(i);
    }

    void push_back(const_reference value) {
        if (count_ >= N) out_of_capacity();
        new (&data_[count_++]) T(value);
    }

    void push_back(rvalue_reference value) {
        if (count_ >= N) out_of_capacity();
        new (&data_[count_++]) T(std::move(value));
    }

    template <typename... Ts>
    reference emplace_back(Ts&&... args) {
        if (count_ >= N) out_of_capacity();
        size_t i = count_++;
        new (&data_[i]) T(std::forward<Ts>(args)...);
        return data_[i];
    }

    void pop_back() { data_[--count_].~T(); }

    void resize(size_type count) {
        size_t i;
        if (count > N) out_of_capacity();
        if (count < count_) {
            for (i = count; i < count_; ++i) {
                data_[i].~T();
            }
        } else if (count_ < count) {
            for (i = count_; i < count; ++i) {
                new (&data_[i]) T();
            }
        }
        count_ = count;
    }

    void resize(size_type count, const_reference value) {
        size_t i;
        if (count > N) out_of_capacity();
        if (count < count_) {
            for (i = count; i < count_; ++i) {
                data_[i].~T();
            }
        } else if (count_ < count) {
            for (i = count_; i < count; ++i) {
                new (&data_[i]) T(value);
            }
        }
        count_ = count;
    }

    template <typename T2, size_t N2, typename Allocator2>
    inline bool operator==(const LimitedVector<T2, N2, Allocator2> v) {
        if (count_ != v.count_) return false;
        for (size_type i = 0; i < count_; ++i) {
            if (data_[i] != v.data_[i]) return false;
        }
        return true;
    }

    template <typename T2, size_t N2, typename Allocator2>
    inline bool operator!=(const LimitedVector<T2, N2, Allocator2> v) {
        if (count_ == v.count_) return true;
        for (size_type i = 0; i < count_; ++i) {
            if (data_[i] != v.data_[i]) return true;
        }
        return false;
    }

   private:
    size_type count_{0};
    allocator_type alloc_;
    T* data_{nullptr};

    iterator get_iterator_(size_type index) { return iterator(data_ + index); }

    const_iterator get_const_iterator_(size_type index) {
        return const_iterator(data_ + index);
    }

    size_t get_index_(iterator it) { return it.ptr_ - data_; }

    size_t get_index_(const_iterator it) { return it.ptr_ - data_; }

    void move_forward_(size_type i, size_type n) {
        if (n == 0) return;
        if (count_ + n >= N) out_of_capacity();
        size_type j;

        for (j = count_ + n - 1; j >= i + n; --j) {
            new (&data_[j]) T(std::move(data_[j - n]));
            data_[j - n].~T();
        }

        count_ += n;
    }

    void move_back_(size_type i, size_type n) {
        if (n == 0) return;
        size_type j;

        for (j = i; j < i + n; ++j) {
            data_[j] = std::move(data_[j + n]);
        }
        for (j = count_ - n; j <= count_; ++j) {
            data_[j].~T();
        }

        count_ -= n;
    }

    [[noreturn]] void out_of_capacity() {
        throw std::length_error("exceeds vector capacity");
    }

    template <typename vT, size_t vN, typename vAllocator>
    friend void std::swap(LimitedVector<vT, vN, vAllocator> v1,
                          LimitedVector<vT, vN, vAllocator> v2);
};

template <typename T>
class LimitedVectorIterator {
   public:
    using iterator = LimitedVectorIterator;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    LimitedVectorIterator() : ptr_(nullptr) {}
    LimitedVectorIterator(pointer ptr) : ptr_(ptr) {}
    LimitedVectorIterator(const iterator& copy) : ptr_(copy.ptr_) {}
    LimitedVectorIterator(iterator&& move) : ptr_(move.ptr_) {}
    inline operator LimitedVectorIterator<const T>() {
        return LimitedVectorIterator<const T>(ptr_);
    }

    inline iterator& operator=(pointer ptr) {
        ptr_ = ptr;
        return *this;
    }
    inline iterator& operator=(const iterator& copy) {
        ptr_ = copy.ptr_;
        return *this;
    }
    inline iterator& operator=(iterator&& move) {
        ptr_ = move.ptr_;
        return *this;
    }
    inline iterator& operator+=(const difference_type& rhs) {
        ptr_ += rhs;
        return *this;
    }
    inline iterator& operator-=(const difference_type& rhs) {
        ptr_ -= rhs;
        return *this;
    }
    inline reference operator*() { return *ptr_; }
    inline pointer operator->() { return ptr_; }
    inline reference operator[](const difference_type& rhs) {
        return ptr_[rhs];
    }

    inline difference_type operator+(const iterator& rhs) {
        return ptr_ + rhs.ptr_;
    }
    inline difference_type operator-(const iterator& rhs) {
        return ptr_ - rhs.ptr_;
    }
    inline iterator operator+(const difference_type& rhs) {
        return iterator(ptr_ + rhs);
    }
    inline iterator operator-(const difference_type& rhs) {
        return iterator(ptr_ - rhs);
    }
    friend inline iterator operator+(const difference_type& lhs,
                                     const iterator& rhs) {
        return iterator(lhs + rhs.ptr_);
    }
    friend inline iterator operator-(const difference_type& lhs,
                                     const iterator& rhs) {
        return iterator(lhs - rhs.ptr_);
    }

    inline iterator& operator++() {
        ++ptr_;
        return *this;
    }
    inline iterator& operator--() {
        --ptr_;
        return *this;
    }
    inline iterator& operator++(int) {
        iterator tmp(*this);
        ++ptr_;
        return tmp;
    }
    inline iterator& operator--(int) {
        iterator tmp(*this);
        --ptr_;
        return tmp;
    }

    inline bool operator==(const iterator& rhs) { return ptr_ == rhs.ptr_; }
    inline bool operator!=(const iterator& rhs) { return ptr_ != rhs.ptr_; }
    inline bool operator>(const iterator& rhs) { return ptr_ > rhs.ptr_; }
    inline bool operator>=(const iterator& rhs) { return ptr_ >= rhs.ptr_; }
    inline bool operator<(const iterator& rhs) { return ptr_ < rhs.ptr_; }
    inline bool operator<=(const iterator& rhs) { return ptr_ <= rhs.ptr_; }

   private:
    pointer ptr_;
    template <typename Tv, size_t N, typename Allocator>
    friend class LimitedVector;
};

};  // namespace hiemalia

namespace std {
template <typename T, size_t N, typename Allocator>
void swap(hiemalia::LimitedVector<T, N, Allocator>& lhs,
          hiemalia::LimitedVector<T, N, Allocator>& rhs) {
    size_t i;

    for (i = 0; i < std::min(lhs.count_, rhs.count_); ++i) {
        swap(lhs[i], rhs[i]);
    }

    if (lhs.count_ < rhs.count_) {
        for (i = lhs.count_; i < rhs.count_; ++i) {
            new (&rhs.data_[i]) T(std::move(&lhs.data_[i]));
            lhs.data_[i].~T();
        }
    } else if (rhs.count_ < lhs.count_) {
        for (i = rhs.count_; i < lhs.count_; ++i) {
            new (&lhs.data_[i]) T(std::move(&rhs.data_[i]));
            rhs.data_[i].~T();
        }
    }

    swap(lhs.alloc_, rhs.alloc_);
    swap(lhs.count_, rhs.count_);
}
}  // namespace std

#endif  // M_LVECTOR_HH
