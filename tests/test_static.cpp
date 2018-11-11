#include "../src/VM.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

cell test_ram[]={
                    OP_PUSH,1,
                    OP_DUP,
                    OP_PUSH,1,
                    OP_ADD,
                    OP_DUP,
                    OP_PUSH,5,
                    OP_EQ,
                    OP_PUSH,2,
                    OP_JZ,
                    OP_HALT,
    };

void print_array(short *arr, int len)
{
    std::cout << std::fixed << std::setfill(' ');
    for (int i=0; i<len/10; i++)
    {
        std::cout<<std::setw(4)<<i<<":";
        for (int j = 0; j<10; j++)
        {
            std::cout<<std::setw(6)<<arr[i*10+j];
        }
        std::cout<<std::endl;
    }
    std::cout<<std::setw(4)<<len/10;
    for (int j = 0; j<len%10; j++)
    {
        std::cout<<std::setw(6)<<arr[(len/10)*10+j];
    }
    std::cout<<std::endl;
}

int main()
{
    VM *m = new VM(test_ram,sizeof(test_ram)/sizeof(cell));

    while (m->running)
    {
        m->printStack();
        m->step();
    }
    Error *err = m->getError();
    if (err->code!=0)
    {
        std::cout<<"Error:"<<std::endl;
        std::cout<<"   Code: "<<err->code<<std::endl;
        std::cout<<"   Location "<<err->location<<std::endl;
        std::cout<<"   Argument: "<<err->param<<std::endl;
        print_array(m->ram,VM_RAM_SIZE);
    }
}
