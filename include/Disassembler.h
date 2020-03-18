//
// Created by chad on 3/1/20.
//

#ifndef YAC8_INTERPRETER_DISASSEMBLER_H
#define YAC8_INTERPRETER_DISASSEMBLER_H

#include <fstream>
#include <iomanip>
#include <iostream>

typedef unsigned char u_char;
typedef unsigned __int16 u_int16_t;

class Disassembler {
  private:
  public:
    void hexDump(char filePath[], std::ostream& out);
    void disassemble(char filePath[], std::ostream& out);
};


#endif  //YAC8_INTERPRETER_DISASSEMBLER_H
