//
// Created by chad on 3/1/20.
//

#include "../include/disassembler.h"
using namespace std;

void disassembler::hexDump(char *filePath) {
    unsigned char buffer[1000];
    int addr = 0;
    int n;
    std::ifstream infile;
    infile.open(filePath);

    // Check if file exists
    if (!infile) {
        cout << "File not found" << endl;
        return;
    }

    while (true) {
        infile.read((char*) buffer, 16);
        // Return buffer size up to 16
        n = infile.gcount();
        if (n <= 0) {
            break;
        }
        // Offset 16 bytes per line
        addr += 16;
        // Print line of n bytes
        for (int i = 0; i < 16; i++) {
            if (i + 1 <= n) {
                cout << hex << setw(2) << setfill('0') << (int)buffer[i];
            }
            // Space each byte
            cout << " ";
        }
        // New line after n bytes
        cout << "\n";
        // Break if end of file
        if (infile.eof()) {
            break;
        }
    }
}
