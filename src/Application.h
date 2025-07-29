#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>

#include <memory>

#include "Display.h"
#include "Interpreter.h"
#include "Shader.h"

namespace Chip8 {

constexpr uint16_t WINDOW_WIDTH = 1920;
constexpr uint16_t WINDOW_HEIGHT = 1080;

class Application {
public:
  Application(const char* rom_location);
  ~Application();

  bool Initialize();
  void Run();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateState();
  void RenderState();

private:
  SDL_Window* m_Window = nullptr;
  SDL_GLContext m_GLContext;

  std::unique_ptr<Interpreter> m_Interpreter;
  std::shared_ptr<Display> m_Display;

  std::unique_ptr<Shader> m_Shader;

  ImVec4 m_BgColor;
  const char* m_RomLocation;

  bool m_StepThrough = true;
  bool m_AdvanceNextStep = false;

  bool m_IsRunning = true;
  unsigned int m_TicksCount;
};
}  // namespace Chip8
