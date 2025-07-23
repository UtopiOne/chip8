#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <stack>

#include "Display.h"

using MemoryAddress = uint16_t;
using Opcode = uint16_t;
using Byte = uint8_t;

constexpr unsigned int MEMORY_SIZE = 4096;
constexpr unsigned int REGISTER_SIZE = 16;

constexpr MemoryAddress FONTSET_START = 0x50;
constexpr int FONTSET_SIZE = 0x50;

constexpr MemoryAddress ROM_START = 0x200;

#define GET_FIRST_NIBBLE(x) x >> 12;
#define GET_SECOND_NIBBLE(x) (x & 0x0F00) >> 8;
#define GET_THIRD_NIBBLE(x) (x & 0x00F0) >> 4;
#define GET_FOURTH_NIBBLE(x) (x & 0x000F);

#define GET_LAST_THREE_NIBBLES(x) (x & 0x0FFF);
#define GET_LAST_TWO_NIBBLES(x) (x & 0x00FF);

namespace Chip8 {

const Byte Font[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

class Interpreter {
public:
  Interpreter(const char *rom_location);

  void Run(float delta_time);

  void SetDisplayPointer(std::shared_ptr<Display> &display_pointer) { m_DisplayPointer = display_pointer; }

  void DumpMemory();

private:
  void LoadROM(const char *rom_location);
  void LoadFont();

private:
  std::shared_ptr<Display> m_DisplayPointer;

  std::array<Byte, MEMORY_SIZE> m_Memory;
  std::array<Byte, REGISTER_SIZE> m_Registers;

  Byte m_DelayTimer, m_SoundTimer;

  MemoryAddress m_ProgramCounter, m_IndexRegister;
  std::stack<MemoryAddress> m_CallStack;

  Opcode m_CurrentOpcode;
};

}  // namespace Chip8
