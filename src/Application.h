#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>

#include <chrono>
#include <memory>

#include "Audio.h"
#include "Display.h"
#include "Interpreter.h"
#include "Shader.h"

namespace Chip8 {

constexpr uint16_t WINDOW_WIDTH = 1280;
constexpr uint16_t WINDOW_HEIGHT = 720;

constexpr unsigned int OPS_PER_SECONDS = 500;

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

  void RenderDebugUI();

private:
  SDL_Window* m_Window = nullptr;
  SDL_GLContext m_GLContext;

  Interpreter m_Interpreter;
  std::shared_ptr<Display> m_Display;
  std::shared_ptr<AudioHandler> m_AudioHandler;

  std::unique_ptr<Shader> m_Shader;

  const char* m_RomLocation;

  bool m_StepThrough = false;
  bool m_AdvanceNextStep = false;

  bool m_IsRunning = true;

  unsigned int m_TicksCount = 0;
  unsigned int m_TicksElapsed = 0;
};
}  // namespace Chip8
