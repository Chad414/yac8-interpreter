#include <iostream>
#include <string>

#include "../include/Disassembler.h"

using namespace std;

char *romPath, *asmOutput;

int main(int argc, char** argv) {
    // Get rom dir from args
    for (int i = 0; i < argc; ++i)
        if (i == 1)
            romPath = argv[i];
        else if(i == 2)
            asmOutput = argv[i];

    std::cout << romPath << std::endl;
    auto* dis = new Disassembler();
    if(asmOutput == NULL)
        dis->disassemble(romPath, cout);    // Output to Console
    else { // Output to File
        cout << "Saving ASM to '" << asmOutput << "'\n";
        ofstream file(asmOutput);
        dis->disassemble(romPath, file);
        file.close();
    }

    // DEBUG: Hex Dump
    cout << "\n\n\nHEX DUMP" << endl;
    dis->hexDump(romPath, cout);



    // Free up Mem
    delete dis;
    return 0;
}