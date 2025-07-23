#include "Interpreter.h"

#include "Logging.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>

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
  this->FetchInstruction();
  this->DecodeInstruction();
  this->ExecuteInstruction();
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

void Interpreter::FetchInstruction() {
  m_CurrentOpcode =
      (m_Memory[m_ProgramCounter] << 8) | (m_Memory[m_ProgramCounter + 1]);

  LOG_TRACE("Current Opcode: {:04X}", m_CurrentOpcode);
  m_ProgramCounter += 2;
}

void Interpreter::DecodeInstruction() {
  auto first_nibble = m_CurrentOpcode >> 12;

  switch (first_nibble) {
  case 0x0: {
    LOG_TRACE("ClearScreen");
    break;
  }
  case 0x1: {
    auto memory_address = m_CurrentOpcode >> 4;
    LOG_TRACE("Jump to: {}", memory_address);
    break;
  }
  case 0x6: {
    auto register_name = (m_CurrentOpcode & 0x0F00) >> 8;
    auto value = (m_CurrentOpcode & 0x00FF);
    LOG_TRACE("Set register V{:X} to {:X}", register_name, value);
    break;
  }
  case 0x7: {
    auto register_name = (m_CurrentOpcode & 0x0F00) >> 8;
    auto value = (m_CurrentOpcode & 0x00FF);
    LOG_TRACE("Add {:X} to register V{:X}", value, register_name);
  }
  case 0xA: {
    auto value = m_CurrentOpcode >> 4;
    LOG_TRACE("Set IndexRegister to {:X}", value);
  }
  }
}

void Interpreter::ExecuteInstruction() {}

} // namespace Chip8
