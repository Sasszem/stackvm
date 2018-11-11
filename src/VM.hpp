#include <stdint.h>
#include <cstring>

#include "opcodes.hpp"
#include "config.hpp"
#include "error.hpp"

class VM
{
    private:
        addr ip =0;
        addr sp =VM_RAM_SIZE - 1;
        addr csp = VM_RAM_SIZE - VM_STACK_SIZE - 1;

        void exec(cell opcode);
        void push_cs(addr ret);
        addr pop_cs();
        void error(cell code, cell param);
        Error err;
    public:
        cell ram[VM_RAM_SIZE];
        VM(cell *ramImage, size_t len);
        VM();
        void step();
        void printStack();
        bool running = true;
        void push(cell s);
        cell pop();
        Error *getError();
};
