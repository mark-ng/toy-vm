# Build a virtual machine

> A simple virtual machine for study purpose

## This machine has :

- 60 bytes of memory
- 5 registers: 
  - 1 for the "program counter"
  - 4 general purpose registers
- 10 instructions (LOAD, STORE, ADD, SUB, ADDI, SUBI, JUMP, BEQ, BEQZ, HALT)
- Two 2-byte little endian Inputs
- One 2-byte little endian Output
- Support negative numbers using 16-bit two's complement
- Can calculate Fibonacci numbers

## dev

Compile and run

```bash
make && ./vm.out
```

Clean up

```bash
make clean
```