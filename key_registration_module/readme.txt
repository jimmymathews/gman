This ncurses-based module performs one-time terminal emulator key configuration and key interpretation at run-time.

Define your own key code names at the top of the file include/key_manager.hpp.
Make sure:
-you include "key_manager.hpp" in the source file where you will handle keys
-"GetPot.h" is accessible to the compiler
-the linker can link to ncurses.

See the test program test.cpp for usage.
