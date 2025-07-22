#pragma once

#include "Display.h"
#include "Interpreter.h"

#include <SDL3/SDL.h>

#include <memory>

namespace Chip8 {

constexpr uint16_t WINDOW_WIDTH = 1920;
constexpr uint16_t WINDOW_HEIGHT = 1080;

class Application {
public:
  Application();
  ~Application();

  bool Initialize(const char *rom_location);
  void Run();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateState();
  void RenderState();

private:
  SDL_Window *m_Window;
  SDL_GLContext m_GLContext;

  std::unique_ptr<Interpreter> m_Interpreter;
  std::unique_ptr<Display> m_Display;

  bool m_IsRunning;
  unsigned int m_TicksCount;
};
} // namespace Chip8
