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

// Op codes
#define LOAD 0x01
#define STORE 0x02
#define ADD 0x03
#define SUB 0x04
#define ADDI 0x05
#define SUBI 0x06
#define JUMP 0x07
#define BEQ 0x08 // Branch if two register are equal
#define BEQZ 0x09 // Branch if register A equal 0
#define HALT 0xFF

// Use offset to calculate the memory address of input, output for extendbility of memory
#define MEMORY_SIZE 50
#define REGISTER_A 0x01
#define REGISTER_B 0x02
#define INPUT_1 (MEMORY_SIZE - 4)
#define INPUT_2 (MEMORY_SIZE - 2)
#define OUTPUT_1 (MEMORY_SIZE - 6)

int main() {
    // Simulate a memory with replacable value
    uint8_t memory[MEMORY_SIZE];

    memset(memory, 0, MEMORY_SIZE);

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
    assert(memory[OUTPUT_1] == 2 && memory[OUTPUT_1 + 1] == 1);
    print_memory(memory, MEMORY_SIZE);

    // TODO: Test negative number
    // 256 - 3 = 253
    uint8_t program_2[MEMORY_SIZE] = {
        LOAD, REGISTER_A, INPUT_1,      // 0x00: load A 0x10
        LOAD, REGISTER_B, INPUT_2,      // 0x03: load B 0x12
        SUB, REGISTER_A, REGISTER_B,    // 0x06: sub A B
        STORE, REGISTER_A, OUTPUT_1,    // 0x09: store A 0x0e
        HALT,                           // 0x0c: halt
    };
    program_2[INPUT_1] = 0x00;
    program_2[INPUT_1 + 1] = 0x01;
    program_2[INPUT_2] = 0x03;
    program_2[INPUT_2 + 1] = 0x00;

    load_program(memory, program_2, MEMORY_SIZE);
    compute(memory, MEMORY_SIZE);
    printf("> Testing 256 - 3 = 253\n");
    assert(memory[OUTPUT_1] == 253 && memory[OUTPUT_1 + 1] == 0);
    print_memory(memory, MEMORY_SIZE);

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
    assert(memory[OUTPUT_1] == 45 && memory[OUTPUT_1 + 1] == 1);
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
    assert(memory[OUTPUT_1] == 43 && memory[OUTPUT_1 + 1] == 1);
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
    assert(memory[OUTPUT_1] == 0x00 && memory[OUTPUT_1 + 1] == 0x00);
    print_memory(memory, MEMORY_SIZE);

    // TODO: Need to wait until memory is larger to fit more instruction
    // uint8_t program_6[memory_size] = {
    //     /*
    //         add from 0 to 10/
    //         first input is set to 10/
    //         store first input in register A/
    //         loop: add to register B the value in register A
    //         subi register A/
    //         beqz (jump to an halt memory address if register A is equal 0) /
    //         jump to loop
    //         halt
    //     */
    //     0x01, 0x01, 0x10,               // 0x00: load A 0x10
    //     0x03, 0x02, 0x01,               // 0x03: add B A
    //     0x06, 0x01,                     // 0x06: subi A
    //     0x09, 0x0c,                     // 0x08: jump to halt if register A is zero
    //     0x07, 0x03,                     // 0x0a: jump back to loop start
    //     0x02, 0x02, 0x0e,               // store register b to output
    //     0xff,                           // 0x0c: halt
    //     0x00, 0x00,                     // 0x0e: output 0
    //     0x0a, 0x00,                     // 0x10: input1 10
    //     0x00, 0x00                      // 0x12: input2 <<unused>>
    // };

    // load_program(memory, program_6, memory_size);
    // compute(memory, memory_size);
    // printf("> Testing JUMP\n");
    // assert(memory[0x0e] == 0x00 && memory[0x0f] == 0x00);
    // print_memory(memory, memory_size);

    printf("OK\n");
}

void compute(uint8_t memory[], unsigned int size) {
    /*
    The memory format is:

    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31
    __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
    INSTRUCTIONS ---------------------------------------------------------------------------------------------------------------------^ OUT-^ IN-1^ IN-2^
    */

    uint16_t registers[3] = { 0x00, 0x00, 0x00 }; // PC, R1, and R2

    uint16_t* pc = &registers[0];

    while (true) {
        uint8_t op = memory[*pc];
        switch (op) {
            case LOAD: {
                uint8_t register_addr = memory[*pc + 1]; // register address to put value in
                uint8_t memory_input_addr = memory[*pc + 2]; // input address to get value

                // Input is 2 byte litte endian
                registers[register_addr] = memory[memory_input_addr + 1] << 8 | memory[memory_input_addr];

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

                registers[register_1_addr] = registers[register_1_addr] + registers[register_2_addr];

                *pc += 3;
                break;
            }
            case SUB: {
                uint8_t register_1_addr = memory[*pc + 1];
                uint8_t register_2_addr = memory[*pc + 2];

                registers[register_1_addr] = registers[register_1_addr] - registers[register_2_addr];
                
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
            case HALT:
                return;
        }
    }
    return;
}

void print_memory(uint8_t memory[], unsigned int size) {
    printf("-----------------------------------------------------------------------memory------------------------------------------------------------------------\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", i);
    }
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", memory[i]);
    }
    printf("\n");
    printf("INSTRUCTIONS ---------------------------------------------------------------------------------------------------------------------^ OUT-^ IN-1^ IN-2^\n");
}

void load_program(uint8_t memory[], uint8_t program[], unsigned int size) {
    printf("> Loading program to memory...\n");
    memcpy(memory, program, size);
    printf("> Program Loaded!\n");
}

