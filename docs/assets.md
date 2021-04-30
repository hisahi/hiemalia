
# Assets
_Hiemalia_'s assets are mostly stored in the `assets` folder. The formats
are mostly text and fairly straightforward in design.

* `demos`: The demo sequences played in the attract mode (arcade mode only)
* `models`: Most 3D models in a pseudo-OBJ format.
* `music`: Game music and `hiemalia.sng` (see below)
* `sections`: Stage sction data files
* `smodels`: Models for stage sections
* `sounds`: Sound effects
* `stages`: Stage scripts

## hiemalia.sng
This file simply contains the list of music tracks used in-game. The format has
one file name per list and the files are assumed to be in the `music` folder.
The track order is as follows:
* Title screen / options music. Meant to be a quiet, mellow ("lo-fi") loop.
* Stage start jingle. Should be less than 5 seconds long and not loop.
* Stage 1-4 music (in order), loop.
* Game complete jingle. Should be less than 8 seconds long and not loop.
* Continue screen tune. Should be less than 11 seconds long and not loop.
* Game over tune. Should be less than 5 seconds long and not loop.
* Name entry theme (if a player gets a high score). Upbeat looping tune.

## TODO: Document other formats?
