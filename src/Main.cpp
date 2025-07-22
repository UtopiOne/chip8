#include <cstdlib>

#include "Application.h"
#include "Logging.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return EXIT_FAILURE;
  }

  Chip8::Logger::Init();
  Chip8::Application application;

  auto rom_location = argv[1];
  if (application.Initialize(rom_location)) {
    application.Run();
  }

  application.Shutdown();

  return EXIT_SUCCESS;
}
