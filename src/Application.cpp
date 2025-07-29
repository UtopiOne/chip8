#include "Application.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>

#include "Logging.h"

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
  const auto window_title =
      std::string("CHIP8: ") + std::filesystem::path(rom_location).filename().string();
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
  glDebugMessageCallback(Logger::OpenGLDebugMessageCallback, nullptr);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();

  ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GLContext);
  ImGui_ImplOpenGL3_Init("#version 330");

  m_Shader = std::make_unique<Shader>();
  m_Shader->Load("src/Shaders/display.vert", "src/Shaders/display.frag");

  m_Interpreter = std::make_unique<Interpreter>(rom_location);
  m_Display = std::make_shared<Display>();
  m_Interpreter->SetDisplayPointer(m_Display);

  m_Display->UpdateDisplayData();

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
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(m_GLContext);
  SDL_DestroyWindow(m_Window);

  SDL_Quit();
}

void Application::ProcessInput() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL3_ProcessEvent(&event);

    switch (event.type) {
      case SDL_EVENT_QUIT: {
        m_IsRunning = false;

        LOG_INFO("Shutdown...");
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
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  static int counter = 0;

  ImGui::Begin("Hello World");
  if (ImGui::Button("Button")) counter++;
  ImGui::Text("%d", counter);

  ImGui::End();

  ImGui::Render();

  glClearColor(155.0 / 255.0, 188.0 / 255.0, 15.0 / 255.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

  m_Shader->SetActive();
  m_Shader->SetUniformMat4(projection, "uProjection");

  m_Display->RenderDisplay();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(m_Window);
}

}  // namespace Chip8
