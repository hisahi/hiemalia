ifndef BACKEND_SDL2
BACKEND_SDL2=1
BASEDEPS := $(BASEDEPS) $(IROOT)/base/sdl2/sdl.hh
BASECXXFLAGS := $(BASECXXFLAGS) $(shell sdl2-config --cflags)
LDLIBS := $(LDLIBS) $(shell sdl2-config --libs) -lSDL2_mixer
endif
