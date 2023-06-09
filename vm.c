#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

// Print memory
void print_memory(uint8_t memory[], unsigned int size);
// Load program to memory
void load_program(uint8_t memory[], uint8_t program[], unsigned int size);
// Virtual machine cpu simulation
void compute(uint8_t memory[], unsigned int size);

int16_t little_endian_to_int16(uint8_t low_order_byte, uint8_t high_order_byte);

// Virtual machine spec
#define MEMORY_SIZE 60 
#define REGISTER_NUM 5 // 1 for the "program counter" and remaining are general purpose register
#define REGISTER_A 0x01
#define REGISTER_B 0x02
#define REGISTER_C 0x03
#define REGISTER_D 0x04

// I/O (2-byte little endian) 
// Use offset to calculate the memory address of input, output for extendbility of memory
#define INPUT_1 (MEMORY_SIZE - 4)
#define INPUT_2 (MEMORY_SIZE - 2)
#define OUTPUT_1 (MEMORY_SIZE - 6)

// Op codes
#define LOAD 0x01   // LOAD reg (addr) : Load value from (addr) to reg
#define STORE 0x02  // STORE reg (addr) : Store value from reg to (addr)
#define ADD 0x03    // ADD reg1 reg2 : Set reg1 to reg1 + reg2 
#define SUB 0x04    // SUB reg1 reg2 : Set reg1 to reg1 - reg2
#define ADDI 0x05   // ADDI reg : Set reg1 = reg1 + 1
#define SUBI 0x06   // SUBI reg : Set reg1 = reg1 - 1
#define JUMP 0x07   // JUMP (addr) : JUMP to (addr)
#define BEQ 0x08    // BEQ reg1 reg2 (addr) : Jump to (addr) if reg1 == reg2
#define BEQZ 0x09   // BEQZ reg (addr) : Jump to (addr) if reg == 0
#define HALT 0xFF   // Halt : End the program

int main() {
    // Simulate a memory with replacable value
    uint8_t memory[MEMORY_SIZE];

    // 255 + 3 = 258
    uint8_t program_1[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,      // 0x00: load A 0x10
        LOAD, REGISTER_B, INPUT_2,      // 0x03: load B 0x12
        ADD, REGISTER_A, REGISTER_B,    // 0x06: add A B
        STORE, REGISTER_A, OUTPUT_1,    // 0x09: store A 0x0e
        HALT,                           // 0x0c: halt
    };
    program_1[INPUT_1] = 0xff;
    program_1[INPUT_1 + 1] = 0x00;
    program_1[INPUT_2] = 0x03;
    program_1[INPUT_2 + 1] = 0x00;

    load_program(memory, program_1, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing 255 + 3 = 258\n");
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == 258);
    print_memory(memory, MEMORY_SIZE);

    // 256 - 300 = -44
    uint8_t program_2[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,      // 0x00: load A 0x10
        LOAD, REGISTER_B, INPUT_2,      // 0x03: load B 0x12
        SUB, REGISTER_A, REGISTER_B,    // 0x06: sub A B
        STORE, REGISTER_A, OUTPUT_1,    // 0x09: store A 0x0e
        HALT,                           // 0x0c: halt
    };
    program_2[INPUT_1] = 0x00;
    program_2[INPUT_1 + 1] = 0x01;
    program_2[INPUT_2] = 0x2C;
    program_2[INPUT_2 + 1] = 0x01;

    load_program(memory, program_2, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing 256 - 300 = -44\n");
    print_memory(memory, MEMORY_SIZE);
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == -44);

    // 300++ = 301
    uint8_t program_3[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,            // 0x00: load A 0x10
        ADDI, REGISTER_A,                     // 0x03: addi A
        STORE, REGISTER_A, OUTPUT_1,          // 0x05: store A 0x0e
        HALT,                                 // 0x08: halt
    };
    program_3[INPUT_1] = 0x2c;
    program_3[INPUT_1 + 1] = 0x01;

    load_program(memory, program_3, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing 300++ = 301\n");
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == 301);
    print_memory(memory, MEMORY_SIZE);

    // 300-- = 299
    uint8_t program_4[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,            // 0x00: load A 0x10
        SUBI, REGISTER_A,                     // 0x03: subi A
        STORE, REGISTER_A, OUTPUT_1,          // 0x05: store A 0x0e
        HALT,                                 // 0x08: halt
    };
    program_4[INPUT_1] = 0x2c;
    program_4[INPUT_1 + 1] = 0x01;

    load_program(memory, program_4, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing 300-- = 299\n");
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == 299);
    print_memory(memory, MEMORY_SIZE);

    // skip some code (do nothing)
    uint8_t program_5[MEMORY_SIZE] = {
        STORE, REGISTER_A, OUTPUT_1,         // 0x00: store A 0x0e
        JUMP, 0x0a,                          // 0x03: jump halt
        SUBI, REGISTER_A,                    // 0x05: subi A
        STORE, REGISTER_A, OUTPUT_1,         // 0x07: store A 0x0e
        HALT,                                // 0x0a: halt
    };
    program_5[OUTPUT_1] = 0x11;
    program_5[OUTPUT_1 + 1] = 0x12;

    load_program(memory, program_5, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing JUMP\n");
    print_memory(memory, MEMORY_SIZE);
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == 0);
    print_memory(memory, MEMORY_SIZE);

    // sum from 0 to 10
    uint8_t program_6[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,               // 0x00: load A 0x10
        ADD, REGISTER_B, REGISTER_A,             // 0x03: add B A
        SUBI, REGISTER_A,                        // 0x06: subi A
        BEQZ, 0x0c,                              // 0x08: jump to output result if register A is zero
        JUMP, 0x03,                              // 0x0a: jump back to loop start
        STORE, REGISTER_B, OUTPUT_1,             // 0x0c: store register b to output
        HALT,                                    // 0x0f: halt
    };
    program_6[INPUT_1] = 0x0a;

    load_program(memory, program_6, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing JUMP\n");
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == 55);
    print_memory(memory, MEMORY_SIZE);

    // Fibonacci
    uint8_t program_7[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,                            // 0x00
        BEQ, REGISTER_A, REGISTER_B, 0x2b,                    // 0x03
        ADDI, REGISTER_C,                                     // 0x07
        BEQ, REGISTER_A, REGISTER_C, 0x2f,                    // 0x09
        SUBI, REGISTER_A,                                     // 0x0d
        ADD, REGISTER_D, REGISTER_B,                          // 0x0f
        ADD, REGISTER_D, REGISTER_C,                          // 0x12
        STORE, REGISTER_C, OUTPUT_1,                          // 0x15
        LOAD, REGISTER_B, OUTPUT_1,                           // 0x18
        STORE, REGISTER_D, OUTPUT_1,                          // 0x1b
        LOAD, REGISTER_D, INPUT_2,                            // 0x1e
        LOAD, REGISTER_C, OUTPUT_1,                           // 0x21
        SUBI, REGISTER_A,                                     // 0x24
        BEQZ, 0x2a,                                           // 0x26
        JUMP, 0x0f,                                           // 0x28
        HALT,                                                 // 0x2a
        STORE, REGISTER_A, OUTPUT_1,                          // 0x2b
        HALT,                                                 // 0x2e
        STORE, REGISTER_C,OUTPUT_1,                           // 0x2f
        HALT,                                                 // 0x32
    };
    
    uint16_t expected[11] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    for (int i = 1; i <= 10; i++) {
        program_7[INPUT_1] = i;
        load_program(memory, program_7, MEMORY_SIZE);
        compute(memory, MEMORY_SIZE);
        printf("> Testing Fibonacci\n");
        assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == expected[i]);
        printf("i = %d ok\n", i);
    }

    // -300 + -200 = -500
    uint8_t program_8[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,      // 0x00: load A 0x10
        LOAD, REGISTER_B, INPUT_2,      // 0x03: load B 0x12
        ADD, REGISTER_A, REGISTER_B,    // 0x06: add A B
        STORE, REGISTER_A, OUTPUT_1,    // 0x09: store A 0x0e
        HALT,                           // 0x0c: halt
    };
    program_8[INPUT_1] = 0xD4;
    program_8[INPUT_1 + 1] = 0xFE;
    program_8[INPUT_2] = 0x38;
    program_8[INPUT_2 + 1] = 0xFF;

    load_program(memory, program_8, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing -300 + -200 = -500\n");
    assert(little_endian_to_int16(memory[OUTPUT_1], memory[OUTPUT_1 + 1]) == -500);
    print_memory(memory, MEMORY_SIZE);
    
    printf("OK\n");
}

void compute(uint8_t memory[], unsigned int size) {
    /*
    The memory format is:

    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 
    01 01 38 01 02 3a 03 01 02 02 01 36 ff 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0c fe d4 fe 38 ff 
    INSTRUCTIONS ---------------------------------------------------------------------------------------------------------------------------------------------------^ OUT-^ IN-1^ IN-2^
    */

    int16_t registers[REGISTER_NUM] = { 0x00, 0x00, 0x00, 0x00, 0x00 }; // PC, R1, and R2, R3, R4

    int16_t* pc = &registers[0];

    while (true) {
        uint8_t op = memory[*pc];
        switch (op) {
            case LOAD: {
                uint8_t register_addr = memory[*pc + 1]; // register address to put value in
                uint8_t memory_input_addr = memory[*pc + 2]; // input address to get value

                // Input is 2 byte litte endian
                registers[register_addr] = little_endian_to_int16(memory[memory_input_addr], memory[memory_input_addr + 1]);

                *pc += 3;
                break;
            }
            case STORE: {
                uint8_t register_addr = memory[*pc + 1]; // register address
                uint8_t memory_output_addr = memory[*pc + 2]; // output address

                // Output is 2 byte litte endian
                memory[memory_output_addr] = registers[register_addr] & 0xFF;
                memory[memory_output_addr + 1] = registers[register_addr] >> 8;

                *pc += 3;
                break;
            }
            case ADD: {
                uint8_t register_1_addr = memory[*pc + 1];
                uint8_t register_2_addr = memory[*pc + 2];

                registers[register_1_addr] = (int16_t) (registers[register_1_addr] + registers[register_2_addr]);

                *pc += 3;
                break;
            }
            case SUB: {
                uint8_t register_1_addr = memory[*pc + 1];
                uint8_t register_2_addr = memory[*pc + 2];

                registers[register_1_addr] = (int16_t) (registers[register_1_addr] - registers[register_2_addr]);
                
                *pc += 3;
                break;
            }
            case ADDI: {
                uint8_t register_1_addr = memory[*pc + 1];

                registers[register_1_addr]++;

                *pc += 2;
                break;
            }
            case SUBI: {
                uint8_t register_1_addr = memory[*pc + 1];

                registers[register_1_addr]--;

                *pc += 2;
                break;
            }
            case JUMP: {
                uint8_t memory_addr = memory[*pc + 1];

                *pc = memory_addr;
                break;
            }
            case BEQZ: {
                uint8_t memory_addr = memory[*pc + 1];
                
                if (registers[1] == 0) {
                    *pc = memory_addr;
                } else {
                    *pc += 2;
                }
                break;
            }
            case BEQ: {
                uint8_t register_1_addr = memory[*pc + 1];
                uint8_t register_2_addr = memory[*pc + 2];
                uint8_t memory_addr = memory[*pc + 3];

                if (registers[register_1_addr] == registers[register_2_addr]) {
                    *pc = memory_addr;
                } else {
                    *pc += 4;
                }
                break;
            }
            case HALT:
                return;
        }
    }
    return;
}

void print_memory(uint8_t memory[], unsigned int size) {
    for (int i = 0; i < size * 3 - 3; i++) {
        printf("-");
    }
    printf("--\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", i);
    }
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", memory[i]);
    }
    printf("\n");
    printf("INSTRUCTIONS ");
    for (int i = 0; i < size * 3 - 1 - 13 - 19; i++) {
        printf("-");
    }
    printf("^ OUT-^ IN-1^ IN-2^\n");
}

void load_program(uint8_t memory[], uint8_t program[], unsigned int size) {
    memset(memory, 0, MEMORY_SIZE);
    printf("> Loading program to memory...\n");
    memcpy(memory, program, size);
    printf("> Program Loaded!\n");
}

int16_t little_endian_to_int16(uint8_t low_order_byte, uint8_t high_order_byte) {
    return (int16_t) (high_order_byte << 8 | low_order_byte);
}

