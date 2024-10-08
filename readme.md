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
| [NES architecture](https://www.copetti.org/writings/consoles/nes/) | detailed article on the architecture of the NES, including diagrams |
| [PPU registers](https://www.nesdev.org/wiki/PPU_registers) | information on the PPU and its registers |
| [CPU memory map](https://www.nesdev.org/wiki/CPU_memory_map) | information on how the CPU maps its memory space |
| [Mappers](https://www.nesdev.org/wiki/Mapper) | list of mappers used in NES carts |
| [CPU power up state](https://www.nesdev.org/wiki/CPU_power_up_state) | details on the state the CPU on startup of the system |
| [PPU power up state](https://www.nesdev.org/wiki/PPU_power_up_state) | details on the state of the PPU on startup of the system |
| [PPU scrolling](https://www.nesdev.org/wiki/PPU_scrolling) | info on how scrolling works, + info on internal PPU registers |

## TODO
- implement PPU
- implement APU
- wire up inputs 
- implement cart mapper abstraction
- implement nrom cart mapper

## wishlist
- debug window with tools like a memory inspector and cart info view
- save states
- menu with options to load a rom, custom colour palette, or save state
