#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

// Print memory
void print_memory(uint8_t memory[], unsigned int size);
// Load program to memory
void load_program(uint8_t memory[], uint8_t program[], unsigned int size);
// Virtual machine cpu simulation
void compute(uint8_t memory[], unsigned int size);

int main() {
    // Simulate a memory with replacable value
    unsigned int memory_size = 20;
    uint8_t memory[memory_size];

    memset(memory, 0, 20);

    // 255 + 3 = 258
    uint8_t program_1[20] = {
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
    printf("Testing 255 + 3 = 258\n");
    assert(memory[0x0e] == 2 && memory[0x0f] == 1);
    print_memory(program_1, memory_size);

    // 256 - 3 = 253
    uint8_t program_2[20] = {
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
    printf("Testing 256 - 3 = 253\n");
    assert(memory[0x0e] == 253 && memory[0x0f] == 0);
    print_memory(memory, memory_size);

    printf("OK");

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

    return;
}

void print_memory(uint8_t memory[], unsigned int size) {
    printf("|============|\n");
    printf("|   MEMORY   |\n");
    printf("|============|\n");
    for (int i = 0; i < size; i++) {
        printf("|0x%02x:\t 0x%02x|\n", i, memory[i]);
    }
    printf("|============|\n");
}

void load_program(uint8_t memory[], uint8_t program[], unsigned int size) {
    printf("> Loading program to memory...\n");
    for (int i = 0; i < size; i++) {
        memory[i] = program[i];
    }
    printf("> Program Loaded!\n");
}

