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

int main() {
    // Simulate a memory with replacable value
    const unsigned int memory_size = 20;
    uint8_t memory[memory_size];

    memset(memory, 0, memory_size);

    // 255 + 3 = 258
    uint8_t program_1[memory_size] = {
        0x01, 0x01, 0x10,  // 0x00: load A 0x10
        0x01, 0x02, 0x12,  // 0x03: load B 0x12
        0x03, 0x01, 0x02,  // 0x06: add A B
        0x02, 0x01, 0x0e,  // 0x09: store A 0x0e
        0xff,              // 0x0c: halt
        0x00,              // 0x0d: <<unused>>
        0x00, 0x00,        // 0x0e: output
        0xff, 0x00,        // 0x10: input X = 255
        0x03, 0x00         // 0x12: input Y = 3
    };

    load_program(memory, program_1, memory_size);
    compute(memory, memory_size);
    printf("> Testing 255 + 3 = 258\n");
    assert(memory[0x0e] == 2 && memory[0x0f] == 1);
    print_memory(memory, memory_size);

    // TODO: Test negative number
    // 256 - 3 = 253
    uint8_t program_2[memory_size] = {
        0x01, 0x01, 0x10,  // 0x00: load A 0x10
        0x01, 0x02, 0x12,  // 0x03: load B 0x12
        0x04, 0x01, 0x02,  // 0x06: sub A B
        0x02, 0x01, 0x0e,  // 0x09: store A 0x0e
        0xff,              // 0x0c: halt
        0x00,              // 0x0d: <<unused>>
        0x00, 0x00,        // 0x0e: output
        0x00, 0x01,        // 0x10: input X = 256
        0x03, 0x00         // 0x12: input Y = 3
    };

    load_program(memory, program_2, memory_size);
    compute(memory, memory_size);
    printf("> Testing 256 - 3 = 253\n");
    assert(memory[0x0e] == 253 && memory[0x0f] == 0);
    print_memory(memory, memory_size);

    // 300++ = 301
    uint8_t program_3[memory_size] = {
        0x01, 0x01, 0x10,               // 0x00: load A 0x10
        0x05, 0x01,                     // 0x03: addi A
        0x02, 0x01, 0x0e,               // 0x05: store A 0x0e
        0xff,                           // 0x08: halt
        0x00, 0x00, 0x00, 0x00, 0x00,   // 0x09: <<unused>>
        0x00, 0x00,                     // 0x0e: output
        0x2c, 0x01,                     // 0x10: input X = 300
        0x00, 0x00                      // 0x12: <<unused>>
    };

    load_program(memory, program_3, memory_size);
    compute(memory, memory_size);
    printf("> Testing 300++ = 301\n");
    assert(memory[0x0e] == 45 && memory[0x0f] == 1);
    print_memory(memory, memory_size);

    // 300-- = 299
    uint8_t program_4[memory_size] = {
        0x01, 0x01, 0x10,               // 0x00: load A 0x10
        0x06, 0x01,                     // 0x03: subi A
        0x02, 0x01, 0x0e,               // 0x05: store A 0x0e
        0xff,                           // 0x08: halt
        0x00, 0x00, 0x00, 0x00, 0x00,   // 0x09: <<unused>>
        0x00, 0x00,                     // 0x0e: output
        0x2c, 0x01,                     // 0x10: input X = 300
        0x00, 0x00                      // 0x12: <<unused>>
    };

    load_program(memory, program_4, memory_size);
    compute(memory, memory_size);
    printf("> Testing 300-- = 299\n");
    assert(memory[0x0e] == 43 && memory[0x0f] == 1);
    print_memory(memory, memory_size);

    // skip some code (do nothing)
    uint8_t program_5[memory_size] = {
        0x02, 0x01, 0x0e,               // 0x00: store A 0x0e
        0x07, 0x0a,                     // 0x03: jump halt
        0x06, 0x01,                     // 0x05: subi A
        0x02, 0x01, 0x0e,               // 0x07: store A 0x0e
        0xff,                           // 0x0a: halt
        0x00, 0x00, 0x00,               // 0x0b: <<unused>>
        0x11, 0x12,                     // 0x0e: output <<junk>>
        0x00, 0x00,                     // 0x10: input <<unused>>
        0x00, 0x00                      // 0x12: <<unused>>
    };

    load_program(memory, program_5, memory_size);
    compute(memory, memory_size);
    printf("> Testing JUMP\n");
    assert(memory[0x0e] == 0x00 && memory[0x0f] == 0x00);
    print_memory(memory, memory_size);

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
    Given a list representing a 20 "byte" array of memory, run the stored
    program to completion, mutating the list in place.

    The memory format is:

    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13
    __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
    INSTRUCTIONS ---------------------------^ OUT-^ IN-1^ IN-2^
    */

    // Op codes
    const uint16_t LOAD = 0x01;
    const uint16_t STORE = 0x02;
    const uint16_t ADD = 0x03;
    const uint16_t SUB = 0x04;
    const uint16_t ADDI = 0x05;
    const uint16_t SUBI = 0x06;
    const uint16_t JUMP = 0x07;
    const uint16_t BEQ = 0x08; // Branch if two register are equal
    const uint16_t BEQZ = 0x09; // Branch if register A equal 0
    const uint16_t HALT = 0xFF;

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
    printf("--------------------------memory---------------------------\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", i);
    }
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("%02x ", memory[i]);
    }
    printf("\n");
    printf("INSTRUCTIONS ---------------------------^ OUT-^ IN-1^ IN-2^\n");
}

void load_program(uint8_t memory[], uint8_t program[], unsigned int size) {
    printf("> Loading program to memory...\n");
    memcpy(memory, program, size);
    printf("> Program Loaded!\n");
}

