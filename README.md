# Chip8-Shenanigans

Various things related to Chip8 programs and emulation
(Not sure how much I'm going to do just yet, mostly for fun)

## How to compile the emulator
```
mkdir build
cd build
cmake ..
make
```

## How to execute ROMs
```
./build/src/chip8_main path_to_ROM_here.ch8
```

When playing, the following keys are used:

```
1 2 3 4
q w e r
a s d f
z x c v
```

To exit the emulation, simply press 'k' once.
