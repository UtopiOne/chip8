#pragma once

#include <cstdint>
#include <stack>

using MemoryAddress = uint16_t;
using Byte = uint8_t;

constexpr unsigned int MEMORY_SIZE = 4096;
constexpr unsigned int REGISTER_SIZE = 16;

namespace Chip8 {

class Interpreter {
public:
  Interpreter(const char *rom_location);

  void Run();

private:
  void LoadROM(const char *rom_location);

  void FetchInstruction();
  void Decode();
  void Execite();

private:
  Byte m_Memory[MEMORY_SIZE];
  Byte m_Registers[REGISTER_SIZE];

  MemoryAddress m_ProgramCounter, m_IndexRegister;
  std::stack<MemoryAddress> m_CallStack;
};

} // namespace Chip8
