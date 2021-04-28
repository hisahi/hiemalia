/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// hholder.hh: header file for HiemaliaExternal (impl in hiemalia.cc)

#ifndef M_HHOLDER_HH
#define M_HHOLDER_HH

#include <memory>
#include <vector>

#include "defs.hh"
#include "inherit.hh"

namespace hiemalia {
class Hiemalia;
class HiemaliaExternal;

class HiemaliaRef {
  public:
    inline HiemaliaRef(Hiemalia& h) : p(h) {}
    DEFAULT_COPY(HiemaliaRef);
    DEFAULT_MOVE(HiemaliaRef);
    ~HiemaliaRef();

    inline Hiemalia& operator*() noexcept { return p.get(); }
    inline Hiemalia* operator->() noexcept { return &(p.get()); }
    Hiemalia& get() noexcept { return p.get(); }

    void bind(HiemaliaExternal*);
    void unbind(HiemaliaExternal*);

  private:
    std::reference_wrapper<Hiemalia> p;
    std::vector<HiemaliaExternal*> ext;
};

class HiemaliaExternal {
  public:
    inline HiemaliaExternal() : p(nullptr) {}
    inline explicit HiemaliaExternal(HiemaliaRef& game) : p(&game) {
        p->bind(this);
    }
    DEFAULT_COPY(HiemaliaExternal);
    DEFAULT_MOVE(HiemaliaExternal);
    ~HiemaliaExternal();

    inline Hiemalia& operator*() {
        if (!p) throw std::logic_error("not bound");
        return *(*p);
    }
    inline Hiemalia* operator->() {
        if (!p) throw std::logic_error("not bound");
        return &(*p).get();
    }

    inline HiemaliaExternal& operator=(std::nullptr_t) noexcept {
        p = nullptr;
        return *this;
    }
    inline operator bool() const noexcept { return p != nullptr; }

    int addCredits(int);

  private:
    HiemaliaRef* p;
};
};  // namespace hiemalia

#endif  // M_HHOLDER_HH
