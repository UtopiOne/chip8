#include "Application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <fmt/base.h>

#include <cstdint>

constexpr uint16_t WINDOW_WIDTH = 1920;
constexpr uint16_t WINDOW_HEIGHT = 1080;

namespace Chip8 {

Application::Application() : m_Window(nullptr), m_IsRunning(true) {}

Application::~Application() {}

bool Application::Initialize() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    fmt::println(stderr, "Failed to initialize SDL: {}", SDL_GetError());

    return false;
  }
  fmt::println("SDL initialized successfully.");

  const auto window_flags = SDL_WINDOW_OPENGL;
  m_Window =
      SDL_CreateWindow("CHIP8", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
  if (m_Window == nullptr) {
    fmt::println(stderr, "SDL_CreateWindow Error: {}", SDL_GetError());

    return false;
  }

  fmt::println("Window initialized successfully.");

  return true;
}

void Application::Run() {
  while (m_IsRunning) {
    ProcessInput();
  }
}

void Application::Shutdown() {
  SDL_DestroyWindow(m_Window);

  SDL_Quit();
}

void Application::ProcessInput() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT: {
      m_IsRunning = false;
      break;
    }
    }
  }
}

} // namespace Chip8
