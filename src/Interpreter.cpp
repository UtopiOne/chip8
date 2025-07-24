#include "Interpreter.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>

#include "Logging.h"

namespace Chip8 {

Interpreter::Interpreter(const char *rom_location)
    : m_IndexRegister(0), m_ProgramCounter(ROM_START) {
  for (int i = 0; i < MEMORY_SIZE; ++i) {
    m_Memory[i] = 0;
  }

  this->LoadFont();
  this->LoadROM(rom_location);
}

void Interpreter::Run(float delta_time) {
  bool increment_program_counter = true;

  m_CurrentOpcode = (m_Memory[m_ProgramCounter] << 8) | (m_Memory[m_ProgramCounter + 1]);

  LOG_TRACE("Current Opcode: {:04X}", m_CurrentOpcode);

  auto first_nibble = GET_FIRST_NIBBLE(m_CurrentOpcode);

  switch (first_nibble) {
    case 0x0: {
      // 00E0: Clear Screen
      if (m_CurrentOpcode == 0x00E0) {
        m_DisplayPointer->ClearDisplay();
        LOG_TRACE("ClearScreen");
        break;
      }
      break;
    }

    // 1NNN: Move (Jump) the program counter to memory address NNN
    case 0x1: {
      auto memory_address = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_ProgramCounter = memory_address;
      increment_program_counter = false;

      LOG_TRACE("Jump to: {:X}", memory_address);
      break;
    }

    // 6XNN: Set the register VX to the value NN
    case 0x6: {
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto value = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);

      m_Registers[register_name] = value;

      LOG_TRACE("Set register V{:X} to {:X}", register_name, value);
      break;
    }

    // 7XNN: Add the value NN to VX
    case 0x7: {
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto value = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);

      m_Registers[register_name] += value;

      LOG_TRACE("Add {:X} to register V{:X}", value, register_name);
      break;
    }

    // ANNN: Set Index Register to the value NNN
    case 0xA: {
      auto value = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_IndexRegister = value;

      LOG_TRACE("Set IndexRegister to {:X}", value);
      break;
    }

    // DXYN: Display N-pixel tall sprite from the index register to the XY
    // location from { VX, VY } registers
    case 0xD: {
      auto x = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto y = GET_THIRD_NIBBLE(m_CurrentOpcode);
      size_t n = GET_FOURTH_NIBBLE(m_CurrentOpcode);

      std::vector<Byte> sprite(n);

      for (int i = m_IndexRegister; i < m_IndexRegister + n; ++i) {
        sprite[i - m_IndexRegister] = m_Memory[i];
      }

      m_DisplayPointer->LoadSprite(m_Registers[x], m_Registers[y], sprite);

      LOG_TRACE("Draw sprite with height {:X} from V{:X}V{:X}", n, x, y);
      break;
    }
    default: {
      LOG_TRACE("Unimplemented or incorrect opcode");
      break;
    }
  }

  if (increment_program_counter) {
    m_ProgramCounter += 2;
  }
}

void Interpreter::DumpMemory() {
  for (int i = 0; i < MEMORY_SIZE; ++i) {
    fmt::print("{:02X} ", m_Memory[i]);
  }
  fmt::println("");
}

void Interpreter::LoadFont() {
  for (int i = 0; i < FONTSET_SIZE; ++i) {
    m_Memory[i + FONTSET_START] = Font[i];
  }
}

void Interpreter::LoadROM(const char *rom_location) {
  std::ifstream input_file(rom_location, std::ios::binary);

  input_file.seekg(0, std::ios::end);
  size_t rom_size = input_file.tellg();
  input_file.seekg(0, std::ios::beg);

  LOG_INFO("ROM location: {}", rom_location);
  LOG_INFO("ROM size: {} bytes", rom_size);

  if (rom_size > MEMORY_SIZE - ROM_START) {
    LOG_ERROR("Failed to read ROM: too big");
  }

  char rom_buffer[rom_size];
  if (input_file.is_open()) {
    input_file.read(&rom_buffer[0], rom_size);
    std::memcpy(&m_Memory[ROM_START], rom_buffer, rom_size);

    input_file.close();
  } else {
    LOG_ERROR("Error: Unable to find/read the input file.");
  }
}

}  // namespace Chip8
