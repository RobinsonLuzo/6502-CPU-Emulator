# 6502 CPU emulator:

Work in Progress.....



Emulation of a Commodore 64 like 8 bit 6502 microporcessor in C++. A contemporary of the Intel 8080, it was introduced in 1975. The chip (and variations) were used in a number of singificant machines including:

- [Apple II](https://en.wikipedia.org/wiki/Apple_II)
- [Nintendo Entertainment System (NES)](https://en.wikipedia.org/wiki/Nintendo_Entertainment_System)
- [Commodore 64](https://en.wikipedia.org/wiki/Commodore_64)
- [BBC Micro](https://en.wikipedia.org/wiki/BBC_Micro)

## Creation:

First, we need to emulate the CPU's [Registers](http://www.obelisk.me.uk/6502/registers.html). There aren't many thankfully - it's much simpler than a modern processor. These are:
- Program Counter: Pointer to the address the CPU is executing the code for.
- Stack Pointer: current part of the stack the program is using. For this it consists of a single 256 byte thread.
- Accumulator: 8 bit register.
- Index Register X: 8 bit register.
- Index Register Y: 8 bit register.
- Processor Status: a single byte with several flags in it that are set to different statuses depending on what's happening. These flags are set or cleared depending on the instruction executing.

An inital boot sequence is made for these.

Secondly, we need memory. This is straighforward enough: 1024 * 64 is the max we can have. Create a method to initialise this on startup to all 0s.

For each tick of the clock we execute a CPU instruction. These [instructions](http://www.obelisk.me.uk/6502/instructions.html) are straighforward enough, though time consuming to implement.



This C++ project uses Visual Studio Code with GCC compiler (g++) and GDB debugger from mingw-w64. [Info](https://code.visualstudio.com/docs/cpp/config-mingw)

## Sources:
- [6502 Wiki](https://en.wikipedia.org/wiki/MOS_Technology_6502)
- [6502 Architecture](http://www.obelisk.me.uk/6502/index.html)