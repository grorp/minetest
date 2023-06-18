This saves the settings twice on shutdown, but as far I've tested, the condition is never triggered anyway:

https://github.com/minetest/minetest/blob/a8cf10b0b523bf4c3234be29b0dd51e719cb00c1/src/client/clientlauncher.cpp#L252-L257

`launch_game` doesn't actually launch a game. It launches the mainmenu and returns a boolean indicating whether we're supposed to launch a game.

`game_has_run` doesn't store whether the game has run, but whether it will run (whether we're supposed to start a game):

https://github.com/minetest/minetest/blob/a8cf10b0b523bf4c3234be29b0dd51e719cb00c1/src/client/clientlauncher.cpp#L233-L234

This sets a setting to which there is no second reference anywhere in the codebase (it was supposedly used by the C++ mainmenu):

https://github.com/minetest/minetest/blob/a8cf10b0b523bf4c3234be29b0dd51e719cb00c1/src/client/clientlauncher.cpp#L443-L444

This ...

1. ... checks whether the gamespec is invalid, and aborts launching a game in that case.
2. ... checks whether `porting::signal_handler_killstatus()` returns a non-null pointer and decides to launch a game in that case. (`porting::signal_handler_killstatus()` always returns a non-null pointer.)
3. ... checks again whether the gamespec is invalid, and aborts launching a game in that case. (Checking again is 100% redundant.) (This never runs anyway because we always already return at step 2.)

https://github.com/minetest/minetest/blob/a8cf10b0b523bf4c3234be29b0dd51e719cb00c1/src/client/clientlauncher.cpp#L507-L524

In the same function a bit above however, the `porting::signal_handler_killstatus()` check is done correctly:

https://github.com/minetest/minetest/blob/a8cf10b0b523bf4c3234be29b0dd51e719cb00c1/src/client/clientlauncher.cpp#L422-L424
