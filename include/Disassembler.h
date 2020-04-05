//
// Created by chad on 3/1/20.
//

#ifndef YAC8_INTERPRETER_DISASSEMBLER_H
#define YAC8_INTERPRETER_DISASSEMBLER_H

#include <fstream>
#include <iomanip>
#include <iostream>

typedef unsigned char u_char;

#ifndef __GNUC__      // GNU C Library
typedef unsigned __int16 u_int16_t;
#endif

class Disassembler {
  private:
  public:
    void hexDump(char filePath[], std::ostream& out);
    void disassemble(char filePath[], std::ostream& out);
};


#endif  //YAC8_INTERPRETER_DISASSEMBLER_H
