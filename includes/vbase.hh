/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// vbase.hh: header file for video module

#ifndef M_VBASE_HH
#define M_VBASE_HH

#include "config.hh"
#include "hbase.hh"
#include "inherit.hh"
#include "module.hh"
#include "sbuf.hh"

namespace hiemalia {
class HostVideoConfig : public ConfigSection {
  public:
  private:
};

class VideoModule : public Module {
  public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }

    virtual bool canSetFullscreen() = 0;

    virtual void frame() = 0;
    virtual void blank() = 0;
    virtual void draw(const SplinterBuffer& buffer) = 0;
    virtual void blit() = 0;
    virtual void sync() = 0;
    virtual bool isFullScreen() = 0;
    virtual void setFullScreen(bool flag) = 0;

    DELETE_COPY(VideoModule);
    INHERIT_MOVE(VideoModule, Module);
    virtual ~VideoModule() noexcept {}

  protected:
    VideoModule() {}

  private:
    static inline const std::string role_ = "video module";
};

std::shared_ptr<VideoModule> getVideoModule(std::shared_ptr<HostModule> host);

};  // namespace hiemalia

#endif  // M_VBASE_HH
