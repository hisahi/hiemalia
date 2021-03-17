/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// video.hh: header file for video engine module

#ifndef M_VIDEO_HH
#define M_VIDEO_HH

#include "config.hh"
#include "module.hh"
#include "msg.hh"
#include "sbuf.hh"
#include "vbase.hh"

namespace hiemalia {
struct VideoMessage {
    int todo_;
};

class VideoEngine : public Module, MessageHandler<VideoMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    VideoEngine(const VideoEngine& copy) = delete;
    VideoEngine& operator=(const VideoEngine& copy) = delete;
    VideoEngine(VideoEngine&& move)
        : Module(std::move(move)), video_(std::move(move.video_)) {}
    VideoEngine& operator=(VideoEngine&& move) {
        Module::operator=(std::move(move));
        video_ = std::move(move.video_);
        return *this;
    }
    VideoEngine(std::shared_ptr<HostModule> host);
    virtual ~VideoEngine() {}

    void frame(const SplinterBuffer& sbuf);
    void sync();

    void gotMessage(const VideoMessage& msg);

   private:
    std::shared_ptr<VideoModule> video_;
    static inline const std::string name_ = "VideoEngine";
    static inline const std::string role_ = "video engine";
};

}  // namespace hiemalia

#endif  // M_VIDEO_HH
