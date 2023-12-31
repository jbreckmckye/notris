# Notris: a block puzzle game for PSOne

Notris is a familiar (but legally distinct!) block puzzle game for the PlayStation 1. It's written in C using PSnoobSDK.

This repo contains builds for three platforms:

- ✅ a JavaScript prototype using `<canvas>`
- ✅ a MacOS C port using SDL
- ⏳ a PlayStation 1 game

# Notes (WIP)

The following are rough notes that will be written up into a blog post

## JavaScript with canvas

`yarn dev-web`

The first implementation is a prototype in a high-level language. Working in JavaScript means we can quickly build an
idea of the overall approach, before moving to C and the more involved SDKs.

Although we're working in JS, we avoid availing ourselves of many high level language features like closures, higher
order functions, or object orientation. The code is meant to resemble the C code we will write later for MacOS and
PlayStation.

## MacOS with C and SDL2

`yarn build-macos && yarn run-macos`

This should compile for windows / Linux too, but I haven't tried it.

We're using SDL2 with the TTF font extension. These need to be installed on the system:

```shell
brew install sdl2 sdl2_ttf
```

Build command is 

```shell
gcc hello.c `sdl2-config --libs` -lm -lSDL2_ttf
```

Explainer for the gcc flags

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name

Set `MallocStackLogging=1` to use `leaks` cmd

Dev notes

- generating tetronimos using numbers
- generating colours using JS

## PlayStation 1

### Setting up dev environment

Compiler etc.
- working on Windows (better emulation options)
- my emulator of choice is Duckstation. It provides some debugging capabilities. The PSX community often recommends no$psx as a dev emulator, but I couldn't get this working
  with my builds.

### `PSn00bSDK`

Rewrite of PSY-Q
Seemed to have the easiest setup & very thorough docs

Requires cmake https://cmake.org/download (I used 3.27.8)
Rest of instructions from https://github.com/Lameguy64/PSn00bSDK
- download binaries for platform, releases here: https://github.com/Lameguy64/PSn00bSDK/releases - I used v0.24
  - downloaded gcc-mipsel-none-elf-12.3.0-windows.zip - this is the MIPS toolchain. Added its /bin to my PATH
  - same dealy with PSn00bSDK-0.24-win32.zip 
  - can check if mkpsxiso (from SDK) and mipsel-none-elf-size (from MIPS toolchain) are recognised on CMD 
- set env var `PSN00BSDK_LIBS` to the libs folder e.g. C:/PSn00bSDK/lib/libpsn00b. Check you can echo `echo %PSN00BSDK_LIBS%` and if not restart shell
- now can invoke compiler with `mipsel-none-elf-gcc`

Then can create project as per https://github.com/Lameguy64/PSn00bSDK/blob/master/doc/installation.md#creating-a-project
Copy template from share\psn00bSDK\template into a new repo
Then call
```
cmake --preset default .
cmake --build ./build
```

To rebuild, clear build dir and re-run commands above

For VSCode, configure the `includePath` in the C/C++ plugin to include `C:\PSn00bSDK/include/libpsn00b` - this enables intellisense for PSX headers

### Showing the PSX logo

Classic PSX games boot in two stages: first they show the Sony Computer Entertainment screen, then the classic PSX logo. But our example project doesn't. Why is that?

Well, the SCEE splash comes from the BIOS, as does the PSX boot sound, but the logo is actually part of the disc license data. It's there to act like a 'seal of authenticity'
in effect.

MKPSXISO supports loading in a license file (i.e. extracted from a retail game via DUMPSXISO), but for the sake of copyright we have to gitignore it.

If we rebuild with a license_data.dat we get the classic splash screen

### Building on windows

Because PSn00bSDK targets windows (and I'm using a windows emulator), I do my scripting for PSX projects with PowerShell. What's the extension of a powershell script?
Why, it's `.ps1`, of course!

There's a `build.ps1` file added to the project that can be executed by right clicking in Explorer

### Drawing

Setup and text http://lameguy64.net/tutorials/pstutorials/chapter1/1-display.html
Drawing and gfx http://lameguy64.net/tutorials/pstutorials/chapter1/2-graphics.html

Basic idea is that the 3D maths, projections etc are handled by a coprocessor called the Geometry Transform Engine, before
the CPU sends "packets" to the GPU to actually go and render / rasterise primitive shapes at particular locations.

Packets are arranged in RAM using an ordering table. This initialises to a simple linked list of n pointers. Primitives are added by inserting structs into the linked list

```
IN ARRAY:

[Front of screen] --> 0 --> 1 --> 2 --> [Back of screen]

LINKED:

0 --                -- 1 <--                                -- 2 <-- BEGINS FROM END 
    \- [Shape 1] <-/         \- [Shape 2] <-- [Shape 3] <--/
```

The ordering table is a reverse linked list. In memory it starts with the packets to display at the front of the screen, and ends with those furthest back, so that the
index in the list is analogous to a z-index. However it is processed back-to-front to implement the "painter's algorithm". When the ordering table is ready it is processed
with `DrawOTag()` passing the _last_ element.

Because primitives need to live longer than the functions that declare them, they need to be initialised in a global buffer instead of the stack. (Otherwise functions returning / entering would overwrite the stack memory). The PSX BIOS and SDKs support heap allocations via `malloc` but are buggy and almost never used.

Each primitive can be between 3 to 13 words (each word is 4 bytes as the PSX is 32 bit). Generally the primitive buffers can just be declared as arrays of `char` and then the programmer uses `sizeof` and pointer arithmetic to work with the memory. The SDK I'm using provides several macros to help with setting up primitives.

Generally two ordering tables and primitive buffers are kept in memory, for the same reason two graphics buffers support performance. One set primitives / linked list can be drawn whilst the other is being prepared for the next frame.

### Displaying text

There is a basic debug text utility embedded in the SDK, but it looks very rough and basic. Instead I opted to create a TIM format texture from some PNG pixel art. I wasn't completely happy with how this looked, but I didn't want to focus all my energy on just displaying text.

You can see the debug and TIM fonts in the image below:

![screenshot](psx-font.png)

### Displaying a play piece

todo

### Controller input

todo

### Integraton