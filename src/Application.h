#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Display.h"
#include "Interpreter.h"
#include "Shader.h"

namespace Chip8 {

constexpr uint16_t WINDOW_WIDTH = 640;
constexpr uint16_t WINDOW_HEIGHT = 320;

class Application {
public:
  Application();
  ~Application();

  bool Initialize(const char* rom_location);
  void Run();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateState();
  void RenderState();

private:
  SDL_Window* m_Window;
  SDL_GLContext m_GLContext;

  std::unique_ptr<Interpreter> m_Interpreter;
  std::shared_ptr<Display> m_Display;

  std::unique_ptr<Shader> m_Shader;

  bool m_IsRunning;
  unsigned int m_TicksCount;
};
}  // namespace Chip8
