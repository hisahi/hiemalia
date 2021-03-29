/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// assetmod.hh: header file for modifying the asset holder

#ifndef M_ASSETMOD_HH
#define M_ASSETMOD_HH

#include "assets.hh"

namespace hiemalia {

// in assets.cc
void setAssetLoadedSounds(std::vector<sound_t>&& sounds);
void setAssetLoadedMusicTracks(std::vector<std::string>&& tracks);

}  // namespace hiemalia

#endif  // M_ASSETMOD_HH
