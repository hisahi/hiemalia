/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// mholder.hh: includes for mholder.cc

#ifndef M_MHOLDER_HH
#define M_MHOLDER_HH

#include <memory>

#include "audio.hh"
#include "defs.hh"
#include "hbase.hh"
#include "input.hh"
#include "logic.hh"
#include "video.hh"

namespace hiemalia {

struct ModuleHolder {
    std::shared_ptr<InputEngine> input;
    std::shared_ptr<VideoEngine> video;
    std::shared_ptr<AudioEngine> audio;
    std::shared_ptr<LogicEngine> logic;

    ModuleHolder(const std::shared_ptr<HostModule> host, GameState& state);
    ~ModuleHolder() noexcept {}
    ModuleHolder(const ModuleHolder& copy) = delete;
    ModuleHolder& operator=(const ModuleHolder& copy) = delete;
    ModuleHolder(ModuleHolder&& move) noexcept;
    ModuleHolder& operator=(ModuleHolder&& move) noexcept;

    void loadAssets();
};

};  // namespace hiemalia

#endif  // M_MHOLDER_HH
