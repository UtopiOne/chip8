#include "Interpreter.h"

#include <cstdio>
#include <filesystem>
#include <fmt/base.h>
#include <fstream>
#include <ios>

namespace Chip8 {

Interpreter::Interpreter(const char *rom_location) : m_IndexRegister(0) {
  for (int i = 0; i < MEMORY_SIZE; ++i) {
    m_Memory[i] = 0;
  }

  this->LoadFont();
  this->LoadROM(rom_location);
}

void Interpreter::Run() {}

void Interpreter::DumpMemory() {
  for (int i = 0; i < MEMORY_SIZE; ++i) {
    fmt::print("{} ", m_Memory[i]);
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
  fmt::println("ROM location: {}", rom_location);
  fmt::println("ROM Size: {} bytes", rom_size);

  if (MEMORY_SIZE - ROM_START - rom_size < 0) {
    fmt::println(stderr, "Failed to read ROM: too big");
  }

  if (input_file.is_open()) {
    for (int i = ROM_START; i < ROM_START + rom_size; ++i) {
      input_file >> m_Memory[i];
    }

    input_file.close();
  } else {
    fmt::println(stderr, "Error: Unable to find/read the input file.");
  }
}

} // namespace Chip8
