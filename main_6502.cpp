#include <stdio.h>
#include <stdlib.h>

// Defining 8 and 16 bit types respectively
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

// Memory
struct Mem {

    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {

        for (u32 i = 0; i < MAX_MEM; i++) {
            Data[i] = 0;
        }
    }

    // Get/Read a Byte out of memory:
    Byte operator[](u32 Address) const {
        return Data[Address];
    }

    // Write 1 byte:
    Byte& operator[](u32 Address) {
        return Data[Address];
    }

    // Write 2 bytes:
    void WriteWord(Word Value, u32 Address) {
        Data[Address]     = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
    }

};


// Main CPU body
struct CPU {
    
    Word PC; // Program counter
    Word SP; // Stack Pointer

    Byte A, X, Y; // Accumulator, Index X and Y

    // Bitfields represent Processor Status - status flags:
    Byte C : 1; // Carry flag
    Byte Z : 1; // Zero flag
    Byte I : 1; // Interrupt Disable
    Byte D : 1; // Decimal mode
    Byte B : 1; // Break Command
    Byte V : 1; // Overflow flag
    Byte N : 1; // Negative flag


    // reset CPU as if the computer was turned on:
    // Boot up seqeunce sets memory to a given point
    void Reset(Mem& memory) {
        
        PC = 0xFFFC;
        SP = 0x0100; // Initialised to 100
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();

    }

    // retrieve instruction byte
    Byte FetchByte(u32& Cycles, Mem& memory) {
        // Go to memory and take the next byte from wherever Program Counter tells us
        Byte Data = memory[PC]; 
        PC++;                  
        Cycles--;   // Move on to next step
        return Data;
    }

    // Fetch a 16 bit word from wherever the program counter is:
    Word FetchWord(u32& Cycles, Mem& memory) {
        Word Data = memory[PC]; // First byte read
        PC++;                  

        // 6502 is little endian so 2nd read matters most
        // Ref: https://stackoverflow.com/questions/29874246/6502-and-little-endian-conversion
        Data |= (memory[PC] << 8); // 2nd byte read - shift up by 8 bytes
        PC++;                  
        Cycles+=2;
        return Data;
    }

    // read instruction byte
    Byte ReadByte(u32& Cycles, Byte Address, Mem& memory) {
        // As no code is being executed no need to increment Program Counter
        // Go straight to given address location
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    // Opcodes:
    static constexpr Byte
        INS_LDA_IM  = 0xA9, // Load Accumulator - immediate:
        INS_LDA_ZP  = 0xA5, // Load Accumulator - zero page:
        INS_LDA_ZPX = 0xB5, // Zero page X - http://www.obelisk.me.uk/6502/addressing.html
        INS_JSR     = 0x20; // Jump to subroutine (16 bit) - http://www.obelisk.me.uk/6502/reference.html#JSR


    // Set zero and exit flags:
    void LDASetStatus() {
        Z = (A == 0);               // Zeros reg set if A=0
        N = (A & 0b10000000) > 0;   // Negative flag set if 7th bit of A is set
    }


    // execute instructions for given number of clock ticks
    void Execute(u32 Cycles, Mem& memory) {

        while (Cycles > 0) {
            // fetch next instruction from memory
            Byte Ins = FetchByte(Cycles, memory);
            switch (Ins) {

                case INS_LDA_IM: {
                    //printf("Switched! %d \n", Ins);
                    Byte Value = FetchByte(Cycles, memory);
                    A = Value;      // Set A register
                    LDASetStatus();
                } break;

                case INS_LDA_ZP: {
                    //printf("Load State: %d \n", Ins);
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LDASetStatus();
                } break;

                // Note: no address overflow protection!!!
                case INS_LDA_ZPX: {
                    //printf("Load X State: %d \n", Ins);
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LDASetStatus();
                } break;

                case INS_JSR: {
                    //printf("Jump: %d \n", Ins);
                    Word SubAddr = FetchWord(Cycles, memory); // subroutine address
                    // Push (address -1) onto the stack
                    memory[SP] = PC - 1; // Stack pointer - program counter positon -1
                    Cycles--;
                    // Set Program Counter to memory address:
                    PC = SubAddr;
                    Cycles--;
                } break;

                default: {
                    printf("Instruction not handled %d", Ins);
                } break;
            } 
            
        }

    }

};


int main() {

    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    // Start - a little inline program
    mem[0xFFFC] = CPU::INS_LDA_ZP;  // Load from zero page
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84; // Stick 84 in the address
    // end - a little inline program
    cpu.Execute(3, mem);

    printf("Hello Everyone");
    return 0;
}