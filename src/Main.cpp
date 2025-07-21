#include <cstdlib>
#include <fmt/base.h>

#include "Application.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fmt::println("Enter filename.");

    return EXIT_FAILURE;
  }

  auto rom_location = argv[1];
  fmt::println("ROM location: {}", rom_location);

  Chip8::Application application;

  if (application.Initialize()) {
    application.Run();
  }

  application.Shutdown();

  return EXIT_SUCCESS;
}
