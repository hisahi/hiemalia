
_Hiemalia_ is written in C++, specifically modern C++ (C++17). I was originally
planning to use C++20, but insufficient compiler support prevented me from
actually achieving this.

# Basic architecture

Internally the game consists of multiple layers. The lowest level is the so
called `base` level, which can technically support any number of possible
backends of four types: host backends (an instance of which is provided for
every other backend), video backends (for graphics), input backends (for
game and menu inputs) and audio backends (for music and sounds). All backends
except for audio are mandatory for the game to run.

In theory, the architecture supports having multiple backends to try and use
when the game starts up. Right now however the build systems are configured such
that only one backend of each type can be compiled in at any given time (except
for audio for which a null backend is always provided as a fallback). Currently
backends are provided for SDL 2 (host, video, input) and SDL_mixer 2 (audio).

# Modules

The game uses a module system. There are modules for handling inputs, video,
audio and game logic (see the next section). These modules depend only on the
backend to provide support and thus should be fine if the backend is changed.

# Game logic

Game logic is implemented by a set of _logic modules_. The main game loop will
call every active logic module every frame with a screen buffer (for drawing)
and the delta (approximate time between this and last call) in seconds.

# Menus

The menu system is a modular menu system with a _menu handler_ (a logic module).
The menu handler itself has a stack of menus and will delegate calls to the
top-level menu, where the automatic code takes over and handles user inputs.
A menu has to implement begin/end functions, the former of which also is
responsible for adding menu options, and possibly a select function that handles
the case where the user selects one of the options.

In addition, menus can also add extra functionality, such as overriding
`renderSpecial` to render special effects on the screen every frame. The main
menu is an example of this.

# Rendering

The rendering in the game revolves around so called "splinters" which are
basically colored lines on the screen. Up until the backend all rendering
happens with floating-point numbers, with the final buffer having XY coordinates
`[-1, 1]`, `[-1, 1]` (anything outside these two is a point outside the screen).
Objects in game are rendered by entering their splinters into the buffer.
3D rendering happens entirely in software with a renderer that projects 3D
points onto the 2D screen. The video backend is finally responsible for actually
drawing the lines on the game surface.

# Objects

The main game (also itself a logic module) then delegates render and simulate
calls to all objects, including the player, enemies and all bullets, and does
the rest of the work.

# Effects

Many of the graphical effects in this game are just nice uses of math, such
as the psychedelic effect in the main menu which is just a combination of some
sine and cosine functions.

Explosion and firework effects show off the "particle" capabilities of the
engine.

# Time spent

Not recorded, but an estimate is 200-250 hours. The time was spent roughly
as follows:

* 30% code
* 15% model design (enemies, stages, etc.)
* 35% stage design
* 15% playtesting
* 5% miscellaneous (sound design, documentation, code cleanup, etc.)
