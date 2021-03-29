/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// module.hh: header file for modules

#ifndef M_MODULE_HH
#define M_MODULE_HH

#include <cassert>
#include <memory>
#include <string>

#include "defs.hh"
#include "logger.hh"

namespace hiemalia {
class Module {
   public:
    virtual ~Module() noexcept {}
    Module(const Module& copy) = delete;
    Module& operator=(const Module& copy) = delete;
    Module(Module&& move) noexcept {}
    Module& operator=(Module&& move) noexcept { return *this; }

    virtual std::string role() const noexcept = 0;
    virtual std::string name() const noexcept = 0;

    template <typename T, typename... Ts>
    static std::shared_ptr<T> create(Ts&&... args) {
        static_assert(std::is_base_of<Module, T>::value, "T must be a Module");
        std::shared_ptr<T> module =
            std::make_shared<T>(std::forward<Ts>(args)...);
        LOG_INFO("started " + module->role() + " '" + module->name() + "'");
        return module;
    }

    template <typename T, typename... Ts>
    static std::unique_ptr<T> create_unique(Ts&&... args) {
        static_assert(std::is_base_of<Module, T>::value, "T must be a Module");
        std::unique_ptr<T> module =
            std::make_unique<T>(std::forward<Ts>(args)...);
        LOG_INFO("started " + module->role() + " '" + module->name() + "'");
        return module;
    }

   protected:
    Module() {}
};
};  // namespace hiemalia

#endif  // M_MODULE_HH
