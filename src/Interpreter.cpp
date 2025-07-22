#include "Interpreter.h"

#include "Logging.h"

#include <cstdio>
#include <filesystem>
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
  // LOG_TRACE("Delta Time: {}", delta_time);

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
  std::ifstream input_file;
  input_file.open(rom_location, std::ios::binary);

  auto rom_size = std::filesystem::file_size(rom_location);
  LOG_INFO("ROM location: {}", rom_location);
  LOG_INFO("ROM size: {} bytes", rom_size);

  if (rom_size > MEMORY_SIZE - ROM_START) {
    LOG_ERROR("Failed to read ROM: too big");
  }

  if (input_file.is_open()) {
    for (int i = ROM_START; i < ROM_START + rom_size; ++i) {
      input_file >> m_Memory[i];
      fmt::print("{} ", m_Memory[i]);
    }

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

void Interpreter::DecodeInstruction() {}

void Interpreter::ExecuteInstruction() {}

} // namespace Chip8
