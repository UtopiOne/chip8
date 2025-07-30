#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <stack>

#include "Audio.h"
#include "Display.h"

using MemoryAddress = uint16_t;
using Opcode = uint16_t;
using Byte = uint8_t;

constexpr MemoryAddress INSTRUCTION_SIZE = 2;

constexpr unsigned int MEMORY_SIZE = 4096;
constexpr unsigned int REGISTER_SIZE = 16;

constexpr MemoryAddress FONTSET_START = 0x50;
constexpr int FONTSET_SIZE = 0x50;

constexpr MemoryAddress ROM_START = 0x200;

constexpr unsigned int FLAG_REGISTER = 0xF;

#define GET_FIRST_NIBBLE(x) x >> 12;
#define GET_SECOND_NIBBLE(x) (x & 0x0F00) >> 8;
#define GET_THIRD_NIBBLE(x) (x & 0x00F0) >> 4;
#define GET_FOURTH_NIBBLE(x) (x & 0x000F);

#define GET_FIRST_BIT(x) (x & 0b10000000) >> 7;
#define GET_LAST_BIT(x) (x & 0b00000001);

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
  void Restart(const char *rom_location);

  void Run();
  void DisplayDebugMenu();

  void SetDisplayPointer(std::shared_ptr<Display> &display_pointer) {
    m_DisplayPointer = display_pointer;
  }
  void SetAudioPointer(std::shared_ptr<AudioHandler> &audio_pointer) {
    m_AudioPoinater = audio_pointer;
  }

private:
  void LoadROM(const char *rom_location);
  void LoadFont();

private:
  std::shared_ptr<Display> m_DisplayPointer;
  std::shared_ptr<AudioHandler> m_AudioPoinater;

  std::array<Byte, MEMORY_SIZE> m_Memory{0};
  std::array<Byte, REGISTER_SIZE> m_Registers{0};

  Byte m_DelayTimer, m_SoundTimer;

  MemoryAddress m_ProgramCounter = ROM_START, m_IndexRegister = 0;
  std::stack<MemoryAddress> m_CallStack;

  Opcode m_CurrentOpcode;

  bool m_DebugStepThrough;
};

}  // namespace Chip8
