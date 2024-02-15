# poNES
yet another NES emulator written in c

## building
use cmake to build the project. eg. with make:

```
cmake -B build
cmake --build build
```

## running
there are two positional arguments the program takes:

1. `rom_path` - the path to the rom to be loaded
2. `palette_path` - the path to the colour palette to be used [optional]

## custom colour palettes
you can load a custom colour palette to be used by passing through a path to the colour palette file in the second
positional argument. the file format is a very simple text file in the following format:

- first line is the name of the palette
- every line proceeding that will have any number of 32bit hex numbers in the RGBA format represented as `#AABBCCDD`,
  totalling up to 64 colours to fill the palette

see `custom.palette` for an example.

## references
| source | description |
|---|---|
| [6502 instruction reference](https://www.nesdev.org/obelisk-6502-guide/reference.html) | reference for the 6502 instruction set present on the NES' ricoh 2A03/2A07 |
| [PPU reference](https://www.nesdev.org/wiki/PPU) | information on the NES' PPU |
| [iNES file format reference](https://www.nesdev.org/wiki/INES) | information on the iNES file format |
| [iNES 2.0 file format reference](https://www.nesdev.org/wiki/NES_2.0) | information on the iNES 2.0 file format |
| [iNES mappers](https://www.nesdev.org/wiki/Mapper#iNES_1.0_mapper_grid) | list of recognised mappers by the iNES file format |
| [NES illegal opcodes](https://www.nesdev.org/wiki/CPU_unofficial_opcodes) | list of undocumented opcodes |
| [6502 addressing modes](https://www.nesdev.org/obelisk-6502-guide/addressing.html) | list of addressing modes instructions will use |
