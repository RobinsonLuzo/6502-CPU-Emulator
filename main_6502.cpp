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
        INS_LDA_IM = 0xA9, // Load Accumulator - immediate:
        INS_LDA_ZP = 0xA5; // Load Accumulator - zero page:


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
                    A = Value;                  // Set A register
                    LDASetStatus();
                } break;
                case INS_LDA_ZP: {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory);
                    A = ReadByte(Cycles, ZeroPageAddress, memory);
                    LDASetStatus();
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
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    // end - a little inline program
    cpu.Execute(2, mem);

    printf("Hello Everyone");
    return 0;
}