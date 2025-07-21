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
  void RenderState();

private:
  SDL_Window *m_Window;
  SDL_GLContext m_GLContext;

  bool m_IsRunning;
};
} // namespace Chip8
