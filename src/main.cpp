#include <iostream>
#include <string>

#include "../include/Disassembler.h"

using namespace std;

char* romPath;

int main(int argc, char** argv) {
    // Get rom dir from args
    for (int i = 0; i < argc; ++i)
        if (i == 1)
            romPath = argv[i];

    std::cout << romPath << std::endl;
    auto* dis = new Disassembler();
    dis->disassemble(romPath);


    // DEBUG: Hex Dump
    cout << "\n\n\nHEX DUMP" << endl;
    dis->hexDump(romPath, cout);



    // Free up Mem
    delete dis;
    return 0;
}