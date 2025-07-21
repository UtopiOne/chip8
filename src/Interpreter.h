#pragma once

#include <cstdint>
namespace Chip8 {

class Interpreter {
public:
  Interpreter();
  ~Interpreter();

private:
  uint8_t m_Memory[4096];

  uint16_t m_ProgramCounter, m_IndexRegister;
};

} // namespace Chip8
