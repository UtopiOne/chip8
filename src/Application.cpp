#include "Application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <glad/glad.h>

#include <filesystem>
#include <memory>

#include "Logging.h"

void MessageCallback(GLenum source, GLenum, GLuint, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: {
      LOG_ERROR("[OPENGL] {}", message);
      break;
    }
    case GL_DEBUG_SEVERITY_MEDIUM: {
      LOG_WARN("[OPENGL] {}", message);
      break;
    }
    case GL_DEBUG_SEVERITY_LOW: {
      LOG_WARN("[OPENGL] {}", message);
      break;
    }
    case GL_DEBUG_SEVERITY_NOTIFICATION: {
      LOG_TRACE("[OPENGL] {}", message);
      break;
    }
    default: {
      LOG_INFO("[OPENGL] {}", message);
      break;
    }
  }
}

namespace Chip8 {

Application::Application() : m_Window(nullptr), m_IsRunning(true) {}

Application::~Application() {}

bool Application::Initialize(const char* rom_location) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    LOG_ERROR("Failed to initialize SDL: {}", SDL_GetError());

    return false;
  }
  LOG_INFO("SDL Initialized successfully.");

  const auto window_flags = SDL_WINDOW_OPENGL;
  const auto window_title = std::string("CHIP8: ") + std::filesystem::path(rom_location).filename().string();
  m_Window = SDL_CreateWindow(window_title.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
  if (m_Window == nullptr) {
    LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());

    return false;
  }
  LOG_INFO("Window initialized successfully.");

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
    LOG_ERROR("SDL_GL_CreateContext error: {}", SDL_GetError());

    return false;
  }
  LOG_INFO("OpenGL context initialized successfully.");

  SDL_GL_MakeCurrent(m_Window, m_GLContext);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD.");

    return false;
  }
  LOG_INFO("GLAD initialized successfully.");

  LOG_TRACE("GL_VERSION: {}", (char*)glGetString(GL_VERSION));
  LOG_TRACE("GL_VENDOR: {}", (char*)glGetString(GL_VENDOR));
  LOG_TRACE("GL_RENDERER: {}", (char*)glGetString(GL_RENDERER));

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, nullptr);

  m_Interpreter = std::make_unique<Interpreter>(rom_location);
  // m_Interpreter->DumpMemory();

  m_Display = std::make_shared<Display>();
  m_Interpreter->SetDisplayPointer(m_Display);

  return true;
}

void Application::Run() {
  while (m_IsRunning) {
    this->ProcessInput();
    this->UpdateState();
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

void Application::UpdateState() {
  float delta_time = (SDL_GetTicks() - m_TicksCount) / 1000.0f;
  m_TicksCount = SDL_GetTicks();

  if (delta_time >= 0.05f) {
    delta_time = 0.05f;
  }

  m_Interpreter->Run(delta_time);
}

void Application::RenderState() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  m_Display->RenderDisplay();

  SDL_GL_SwapWindow(m_Window);
}

}  // namespace Chip8
