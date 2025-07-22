#include "Interpreter.h"
#include <cstdio>
#include <filesystem>
#include <fmt/base.h>
#include <fstream>
#include <ios>

namespace Chip8 {

Interpreter::Interpreter(const char *rom_location) { LoadROM(rom_location); }

void Interpreter::LoadROM(const char *rom_location) {
  std::ifstream input_file;
  input_file.open(rom_location, std::ios::binary);

  auto rom_size = std::filesystem::file_size(rom_location);
  fmt::println("ROM location: {}", rom_location);
  fmt::println("ROM Size: {} bytes", rom_size);

  if (input_file.is_open()) {
    for (int i = 512; i < 512 + rom_size; ++i) {
      input_file >> m_Memory[i];
    }

    input_file.close();
  } else {
    fmt::println(stderr, "Error: Unable to find/read the input file.");
  }
}

void Interpreter::Run() {}

} // namespace Chip8
