#pragma once

#include <SDL3/SDL.h>

namespace Chip8 {
class Application {
public:
  Application();
  ~Application();

  bool Initialize();
  void Run();
  void Shutdown();

private:
  void ProcessInput();

private:
  SDL_Window *m_Window;

  bool m_IsRunning;
};
} // namespace Chip8
