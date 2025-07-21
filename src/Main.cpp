#include <cstdlib>

#include "Application.h"

int main(int argc, char *argv[]) {
  Chip8::Application application;

  if (application.Initialize()) {
    application.Run();
  }

  application.Shutdown();

  return EXIT_SUCCESS;
}
