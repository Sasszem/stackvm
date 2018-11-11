
#include "../src/VM.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

int readFile(const char * fname, short * array, unsigned length)
{
    std::fstream file(fname, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cout<<"Could not open file: \""<<fname<<"\""<<std::endl;
        return 0;
    }

    int file_size = file.tellg();
    if (file_size/sizeof(short)<=length)
    {
        file.seekg(0);
        for (int i=0; i<file_size/sizeof(short); i++)
        {
            short f;
            file.read((char*)&f,2);
            std::cout<<(int)f<<std::endl;
            array[i]=f;
        }
    }
    else
    {
        std::cout<<"File is too big for loading"<<std::endl;
        return 0;
        
    }
    return file_size/sizeof(short);
}

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

int main(int argc, char* argv[])
{
    if (argc==1)
    {
        std::cout<<"Usage: "<<argv[0]<<" <inputfile>"<<std::endl;
        return 0;
    }
    
    short buff[256];

    int len = readFile(argv[1],buff,256);
    if (len==0)
        return 0;
    VM *m = new VM(buff,len);

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
