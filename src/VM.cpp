// Copyright 2018 Sasszem
#include <iostream>
#include <cstring>
using std::cout;
using std::endl;
#include "VM.hpp"

VM::VM(cell *ramImage, size_t len)
{
    memcpy(this->ram, ramImage, len*sizeof(short));
}
VM::VM()
{
}

void VM::step()
{
    if (running)
    {
        char opcode = ram[ip];
        this->exec(opcode);
        ip++;
    }
}

void VM::exec(cell opcode)
{
    cout << "Executing opcode: " << opcode << endl;
    cout << "ip=" << ip << endl;
    cout << "sp=" << sp << endl;
    cout << "csp=" << csp << endl;
    switch (opcode)
    {
        // Stack manipulation
        case OP_PUSH:
        {
            cell val = ram[ip+1];
            push(val);
            ip++;
            break;
        }
        case OP_DROP:
        {
            pop();
            break;
        }
        case OP_DUP:
        {
            cell val = pop();
            push(val);
            push(val);
            break;
        }
        case OP_SWAP:
        {
            cell val = pop();
            cell val2 = pop();
            push(val);
            push(val2);
            break;
        }
        case OP_ROT:
        {
            cell val = pop();
            cell val2 = pop();
            cell val3 = pop();
            push(val2);
            push(val);
            push(val3);
            break;
        }

        // Arithmetic

        case OP_ADD:
        {
            cell ret = pop()+pop();
            push(ret);
            break;
        }
        case OP_SUB:
        {
            cell second = pop();
            cell first = pop();
            cell ret = first-second;
            push(ret);
            break;
        }
        case OP_MUL:
        {
            cell ret = pop()*pop();
            push(ret);
            break;
        }
        case OP_DIV:
        {
            cell second = pop();
            cell first = pop();

            cell ret = 0;
            if (second != 0)
            {
                ret = first/second;
            }
            else  // div by zero!
            {
                error(ERROR_DIV_BY_ZERO, 0);
                break;
            }

            push(ret);
            break;
        }
        case OP_MOD:
        {
            cell second = pop();
            cell first = pop();

            if (second == 0)  // mod by zero
            {
                error(ERROR_MOD_BY_ZERO, 0);
                break;
            }

            cell ret = first%second;
            push(ret);
            break;
        }

        // Coparison ops:

        case OP_EQ:
        {
            cell ret = ((pop() == pop()) ? 1 : 0);
            push(ret);
            break;
        }
        case OP_GT:
        {
            cell ret = ((pop() < pop()) ? 1 : 0);
            push(ret);
            break;
        }
        case OP_LT:
        {
            cell ret = ((pop() > pop()) ? 1 : 0);
            push(ret);
            break;
        }

        // RAM:

        case OP_STOR:
        {
            cell adr = pop();
            cell val = pop();
            if (adr >= 0 && adr < VM_RAM_SIZE)
                ram[adr] = val;
            else
            {
                error(ERROR_ILL_ADR, adr);
            }
            break;
        }

        case OP_LOAD:
        {
            cell adr = pop();
            if (adr >= 0 && adr < VM_RAM_SIZE)
                push(ram[adr]);
            else
            {
                error(ERROR_ILL_ADR, adr);
            }
            break;
        }

        // Jumping
        case OP_GOTO:
        {
            cell adr = pop();
            if (adr >= 0 && adr < VM_RAM_SIZE)
                ip = adr - 1;
            else
            {
                error(ERROR_ILL_ADR, adr);
            }
            break;
        }

        case OP_JZ:
        {
            cell adr = pop();
            if (adr >= 0 && adr < VM_RAM_SIZE)
            {
                if (pop() == 0)
                {
                    ip = adr-1;
                }
            }
            else
            {
                error(ERROR_ILL_ADR, adr);
            }
            break;
        }

        case OP_CALL:
        {
            addr to = pop();
            if (to >= 0 && to < VM_RAM_SIZE)
            {
                push_cs(ip);
                ip = to - 1;
            }
            else
            {
                error(ERROR_ILL_ADR, to);
            }
            break;
        }

        case OP_RET:
        {
            addr to = pop_cs();
            ip = to;
            break;
        }

        // IO

        case OP_OUTCHAR:
        {
            outChar(pop());
            break;
        }



        // Misc:
        case OP_HALT:
        {
            error(ERROR_NO_ERROR, 0);
            break;
        }

        default:
            cout << "Unknown opcode: " << opcode << endl;
            error(ERROR_UNKNOWN_OPCODE, opcode);
            break;
    }
}

void VM::printStack()
{
    for (addr i = sp + 1; i < VM_RAM_SIZE; i++)
    {
        cout << ram[i] << " ";
    }
    cout << endl;
}


cell VM::pop()
{
    if (sp < VM_RAM_SIZE-1)
    {
        sp++;
        return ram[sp];
    }
    else
    {
        if (VM_STACK_CAN_UNDERFLOW)
        {
            return 0;
        }
        else
        {
            error(ERROR_STACK_UNDERFLOW, sp);
        }
    }
}

void VM::push(cell s)
{
    if (sp >= (VM_RAM_SIZE-VM_STACK_SIZE))
    {
        ram[sp] = s;
        sp--;
    }
    else
    {
        error(ERROR_STACK_OVERFLOW, 0);
    }
}

void VM::error(cell code, cell param)
{
    running = false;
    err.code = code;
    err.param = param;
    err.location = ip;
}

Error* VM::getError()
{
    return &err;
}

addr VM::pop_cs()
{
    if (csp < VM_RAM_SIZE - VM_STACK_SIZE - 1)
    {
        csp++;
        return ram[csp];
    }
    else
    {
        error(ERROR_CALL_STACK_UNDERFLOW, csp);
    }
}

void VM::push_cs(addr s)
{
    if (csp >= (VM_RAM_SIZE - VM_STACK_SIZE - VM_CALL_STACK_SIZE))
    {
        ram[csp] = s;
        csp--;
    }
    else
    {
        error(ERROR_CALL_STACK_OVERFLOW, 0);
    }
}
