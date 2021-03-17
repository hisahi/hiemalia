/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// hbase.hh: header file for host module

#ifndef M_HBASE_HH
#define M_HBASE_HH

#include "config.hh"
#include "module.hh"
#include "sbuf.hh"

namespace hiemalia {
class HostModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }
    virtual void begin() = 0;
    virtual bool proceed() = 0;
    virtual void quit() = 0;
    virtual void finish() = 0;

    HostModule(const HostModule& copy) = delete;
    HostModule& operator=(const HostModule& copy) = delete;
    HostModule(HostModule&& move) : Module(std::move(move)) {}
    HostModule& operator=(HostModule&& move) {
        Module::operator=(std::move(move));
        return *this;
    }
    virtual ~HostModule() {}

   protected:
    HostModule() {}

   private:
    static inline const std::string role_ = "host module";
};

std::shared_ptr<HostModule> getHostModule();

};  // namespace hiemalia

#endif  // M_HBASE_HH
