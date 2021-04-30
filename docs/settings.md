
# Settings
_Hiemalia_ features a variety of different settings, some of which may be
implementation-specific.

Game settings allows configuring the initial difficulty (easy, medium, hard) as
well as the number of continues the player starts out with.

Video settings allows changing between full screen and windowed modes.

Audio settings allows enabling or disabling sound effects and music.

Control settings allows remapping all in-game controls (menu controls however
cannot be presently adjusted).

By default, the game stores settings in a file called `hiemalia.cfg` in the
working directory. This can be changed with the `--config` flag.

# Arcade mode
If the game is run with the `--arcade` flag, it enters a special mode called
the "arcade mode". The main menu is replaced with an automatic screen that
changes between the game logo, high score screen, help screens and an attract
mode (automatic demo). In order to play, the player must insert credits (done
by pressing F1), or the game must be configured to be in free play mode from
the config file. The arcade mode is also automatically in full screen, and can
be exited by pressing Alt+F4 (as no main menu "quit" option is available).

# Flags
Hiemalia has a variety of other flags too. They can all be seen by using
the `--help` flag.
