/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2.hh: header file for SDL2 base

#ifndef M_BASE_SDL2_HH
#define M_BASE_SDL2_HH

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>

#include <sstream>
#include <stdexcept>

namespace hiemalia {

inline std::string SDL2_build_message(const std::string& message) {
    std::ostringstream stream;
    stream << message << ": " << SDL_GetError();
    return stream.str();
}

inline std::string SDLmixer2_build_message(const std::string& message) {
    std::ostringstream stream;
    stream << message << ": " << Mix_GetError();
    return stream.str();
}

class SDLBaseException : public std::runtime_error {
   public:
    SDLBaseException(const std::string& message)
        : std::runtime_error(message) {}
};

class SDLException : public SDLBaseException {
   public:
    SDLException(const std::string& message)
        : SDLBaseException(SDL2_build_message(message)) {}
};

class SDLMixerException : public SDLBaseException {
   public:
    SDLMixerException(const std::string& message)
        : SDLBaseException(SDLmixer2_build_message(message)) {}
};

};  // namespace hiemalia

#endif  // M_BASE_SDL2_HH
