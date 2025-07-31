#include <cstdlib>
#include <ctime>

#include "Application.h"
#include "Logging.h"

int main(int argc, char *argv[]) {
  srand(time(nullptr));

  if (argc == 1) {
    return EXIT_FAILURE;
  }

  auto rom_location = argv[1];

  Chip8::Logger::Init();
  Chip8::Application application(rom_location);

  if (application.Initialize()) {
    application.Run();
  }

  application.Shutdown();

  return EXIT_SUCCESS;
}
