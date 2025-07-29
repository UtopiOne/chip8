#include "Interpreter.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
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

  LOG_TRACE("{} Current Opcode: {:04X}", m_ProgramCounter, m_CurrentOpcode);

  auto first_nibble = GET_FIRST_NIBBLE(m_CurrentOpcode);

  switch (first_nibble) {
    case 0x0: {
      // 00E0: Clear Screen
      if (m_CurrentOpcode == 0x00E0) {
        m_DisplayPointer->ClearDisplay();
        LOG_TRACE("ClearScreen");
        break;
      }

      // 00EE: Return from a subroutine
      if (m_CurrentOpcode == 0x00EE) {
        auto memory_address = m_CallStack.top();
        m_CallStack.pop();

        m_ProgramCounter = memory_address;

        LOG_TRACE("Subroutine returned, memory address set to {}", memory_address);
        break;
      }
      break;
    }

    // 1NNN: Move (Jump) the program counter to memory address NNN
    case 0x1: {
      MemoryAddress memory_address = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_ProgramCounter = memory_address;
      increment_program_counter = false;

      LOG_TRACE("Jump to: {:X}", memory_address);
      break;
    }

    // 2NNN: Call subroutine at NNN
    case 0x2: {
      m_CallStack.push(m_ProgramCounter);

      MemoryAddress memory_address = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_ProgramCounter = memory_address;

      increment_program_counter = false;

      LOG_TRACE("Called subroutine at memory address {}", memory_address);
      break;
    }

    // 3XNN: Increment program counter by 2 if Vx == NN
    case 0x3: {
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto value = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);

      if (m_Registers[register_name] == value) {
        m_ProgramCounter += INSTRUCTION_SIZE;
      }

      LOG_TRACE("Skip if V{} == {} ({})", register_name, value,
                m_Registers[register_name] == value);

      break;
    }

    // 4XNN: Increment program counter by 2 if Vx != NN
    case 0x4: {
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto value = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);

      if (m_Registers[register_name] != value) {
        m_ProgramCounter += INSTRUCTION_SIZE;
      }

      LOG_TRACE("Skip if V{} != {} ({})", register_name, value,
                !(m_Registers[register_name] == value));

      break;
    }

    // 5XY0: Increment program counter by 2 if Vx == Vy
    case 0x5: {
      auto register_name_x = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto register_name_y = GET_THIRD_NIBBLE(m_CurrentOpcode);

      if (m_Registers[register_name_x] == m_Registers[register_name_y]) {
        m_ProgramCounter += INSTRUCTION_SIZE;
      }

      LOG_TRACE("Skip if V{} == V{} ({})", register_name_x, register_name_y,
                m_Registers[register_name_x] == m_Registers[register_name_y]);

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

    // 8XYN: Logical and arithmetic instructions
    case 0x8: {
      auto type = GET_FOURTH_NIBBLE(m_CurrentOpcode);
      auto register_name_x = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto register_name_y = GET_THIRD_NIBBLE(m_CurrentOpcode);

      switch (type) {
        // 8XY0: Set
        case 0x0: {
          m_Registers[register_name_x] = m_Registers[register_name_y];
          LOG_TRACE("Set V{} to the value of V{}: {}", register_name_x, register_name_y,
                    m_Registers[register_name_y]);
          break;
        }

        // 8XY1: Binary OR
        case 0x1: {
          m_Registers[register_name_x] |= m_Registers[register_name_y];
          LOG_TRACE("Binary OR V{} and V{}", register_name_x, register_name_y);

          break;
        }

        // 8XY2: Binary AND
        case 0x2: {
          m_Registers[register_name_x] &= m_Registers[register_name_y];
          LOG_TRACE("Binary AND V{} and V{}", register_name_x, register_name_y);

          break;
        }

        // 8XY3: Logical XOR
        case 0x3: {
          m_Registers[register_name_x] ^= m_Registers[register_name_y];
          LOG_TRACE("Binary XOR V{} and V{}", register_name_x, register_name_y);

          break;
        }

        // 8XY4: Add
        case 0x4: {
          m_Registers[register_name_x] += m_Registers[register_name_y];
          LOG_TRACE("Add V{} and V{}", register_name_x, register_name_y);

          break;
        }

        // 8XY5: Subtract VX - VY
        case 0x5: {
          m_Registers[FLAG_REGISTER] = 0;
          if (m_Registers[register_name_x] > m_Registers[register_name_y]) {
            m_Registers[FLAG_REGISTER] = 1;
          }

          m_Registers[register_name_x] -= m_Registers[register_name_y];
          LOG_TRACE("Subtracted V{} - V{}", register_name_x, register_name_y);

          break;
        }

        // 8XY6: Shift right
        case 0x6: {
          m_Registers[register_name_x] = m_Registers[register_name_y];

          Byte flag = GET_LAST_BIT(m_Registers[register_name_x]);

          m_Registers[register_name_x] >>= 1;

          m_Registers[FLAG_REGISTER] = flag;
          LOG_TRACE("Set V{} to V{} and shifted 1 bit right", register_name_x, register_name_y);

          break;
        }

        // 8XY7: Subtract VY - VX
        case 0x7: {
          m_Registers[FLAG_REGISTER] = 0;
          if (m_Registers[register_name_y] > m_Registers[register_name_x]) {
            m_Registers[FLAG_REGISTER] = 1;
          }

          m_Registers[register_name_x] =
              m_Registers[register_name_y] - m_Registers[register_name_x];
          LOG_TRACE("Subtracted V{} - V{}", register_name_y, register_name_x);

          break;
        }

        // 8XYE: Shift left
        case 0xE: {
          m_Registers[register_name_x] = m_Registers[register_name_y];

          Byte flag = GET_FIRST_BIT(m_Registers[register_name_x]);

          m_Registers[register_name_x] <<= 1;

          m_Registers[FLAG_REGISTER] = flag;
          LOG_TRACE("Set V{} to V{} and shifted 1 bit left", register_name_x, register_name_y);

          break;
        }
      }
      break;
    }

    // 9XY0: Increment program counter by 2 if Vx != Vy
    case 0x9: {
      auto register_name_x = GET_SECOND_NIBBLE(m_CurrentOpcode);
      auto register_name_y = GET_THIRD_NIBBLE(m_CurrentOpcode);

      if (m_Registers[register_name_x] != m_Registers[register_name_y]) {
        m_ProgramCounter += INSTRUCTION_SIZE;
      }

      LOG_TRACE("Skip if V{} != V{} ({})", register_name_x, register_name_y,
                !(m_Registers[register_name_x] == m_Registers[register_name_y]));

      break;
    }

    // ANNN: Set Index Register to the value NNN
    case 0xA: {
      auto value = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_IndexRegister = value;

      LOG_TRACE("Set IndexRegister to {:X}", value);
      break;
    }

    // BNNN: Jump with offset
    case 0xB: {
      auto address = GET_LAST_THREE_NIBBLES(m_CurrentOpcode);
      m_ProgramCounter = address + m_Registers[0x0];
      increment_program_counter = false;

      LOG_TRACE("Jumped to {} with offset {}", address, m_Registers[0x0]);

      break;
    }

    // CXNN: Generate random number & NN into Vx
    case 0xC: {
      auto nn = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);

      Byte number = std::rand();

      m_Registers[register_name] = number & nn;
      LOG_TRACE("Generated random value {} for V{}", m_Registers[register_name], register_name);

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

      auto flag = m_DisplayPointer->LoadSprite(m_Registers[x], m_Registers[y], sprite);
      m_DisplayPointer->UpdateDisplayData();

      m_Registers[FLAG_REGISTER] = (Byte)flag;

      LOG_TRACE("Draw sprite with height {:X} at {} {}", n, m_Registers[x], m_Registers[y]);
      break;
    }

    case 0xF: {
      auto type = GET_LAST_TWO_NIBBLES(m_CurrentOpcode);
      auto register_name = GET_SECOND_NIBBLE(m_CurrentOpcode);

      switch (type) {
        // FX1E: I = I + Vx
        case 0x1E: {
          m_IndexRegister += m_Registers[register_name];
          LOG_TRACE("I += V{}", register_name);
          break;
        }
      }

      break;
    }

    default: {
      LOG_WARN("Unimplemented or incorrect opcode");
      break;
    }
  }

  if (increment_program_counter) {
    m_ProgramCounter += INSTRUCTION_SIZE;
  }
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
