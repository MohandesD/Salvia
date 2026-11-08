# COMPILING
To compile salvia, three .bat files are provided to generate the libraries and final executables. The order should be the following:

```
Salvia
├── 1. .\libs\rebuildAllWin.bat and libs\rebuildAllXbox.bat (Compile common libraries for salvia)
├── 2. .\libretro\rebuildAll.bat (Compile each libretro core into an independent library)
└── 3. .\rebuildAllWin.bat and rebuildAllXbox.bat (Link each core and integrate them into the salvia frontend)
```

## REQUISITES
IDE and libraries needed:
- Visual Studio 2010 and Service pack 1
- Xbox 360 Extensions for Visual Studio 2.0.21256.0

## WHY SO MANY XEX'S?
Due to the architecture of xbox 360 each core can't be compiled into a dynamic library and loaded in runtime, so each .xex file integrates the frontend and each core code.

## WINDOWS APPLICATION VS XBOX .XEX

`rebuildAllWin.bat` is the Windows frontend (Visual Studio 2010, Win32). `rebuildAllXbox.bat` is the Xbox 360 `.xex` path and needs the Xbox 360 Extensions for Visual Studio named above. A machine without that XDK can compile neither the `.xex` nor, unless Visual Studio 2010 itself is installed, `Salvia.exe`. Do not substitute an unofficial or leaked copy of the Xbox 360 SDK.

The theme parser does not need either toolchain. From the repository root, with a C++03 compiler:

```
g++ -std=c++03 -Wall -Wextra -Werror -pedantic -I src tests/theme_test.cpp src/theme/theme.cpp -o tests/theme_test
./tests/theme_test
```

That test only checks `theme.cfg` parsing and the shipped `default` and `emulationstation` themes. It does not draw the frontend and it does not replace a Windows or Xbox build.
