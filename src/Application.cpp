#include "Application.h"

#include <SDL3/SDL.h>
#include <fmt/base.h>
#include <glad/glad.h>

#include <memory>

void MessageCallback(GLenum source, GLenum, GLuint, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam) {
  fmt::println(stderr, "OpenGL: {}", message);
}

namespace Chip8 {

Application::Application() : m_Window(nullptr), m_IsRunning(true) {}

Application::~Application() {}

bool Application::Initialize(const char *rom_location) {
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

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  m_GLContext = SDL_GL_CreateContext(m_Window);
  if (m_GLContext == nullptr) {
    fmt::println(stderr, "SDL_GL_CreateContext error: {}", SDL_GetError());

    return false;
  }
  fmt::println("OpenGL context initialized successfully.");

  SDL_GL_MakeCurrent(m_Window, m_GLContext);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    fmt::println(stderr, "Failed to initialize GLAD.");

    return false;
  }
  fmt::println("GLAD initialized successfully");

  fmt::println("GL_VERSION: {}", (char *)glGetString(GL_VERSION));
  fmt::println("GL_VENDOR: {}", (char *)glGetString(GL_VENDOR));
  fmt::println("GL_RENDERER: {}", (char *)glGetString(GL_RENDERER));

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, nullptr);

  m_Interpreter = std::make_unique<Interpreter>(rom_location);
  m_Display = std::make_unique<Display>();

  m_Interpreter->DumpMemory();

  return true;
}

void Application::Run() {
  while (m_IsRunning) {
    this->ProcessInput();
    this->RenderState();
  }
}

void Application::Shutdown() {
  SDL_DestroyWindow(m_Window);
  SDL_GL_DestroyContext(m_GLContext);

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

void Application::RenderState() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  m_Display->RenderDisplay();

  SDL_GL_SwapWindow(m_Window);
}

} // namespace Chip8
