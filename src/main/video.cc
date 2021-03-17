/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// video.cc: implementation of VideoEngine

#include "video.hh"

#include "hbase.hh"
#include "vbase.hh"

namespace hiemalia {
VideoEngine::VideoEngine(std::shared_ptr<HostModule> host)
    : video_(std::move(getVideoModule(std::move(host)))) {}

void VideoEngine::gotMessage(const VideoMessage& msg) {}

void VideoEngine::frame(const SplinterBuffer& sbuf) {
    video_->frame();
    video_->blank();
    video_->draw(sbuf);
    video_->blit();
}

void VideoEngine::sync() { video_->sync(); }

}  // namespace hiemalia
