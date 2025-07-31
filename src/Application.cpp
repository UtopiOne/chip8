#include "Application.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <cmath>
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>

#include "Audio.h"
#include "Logging.h"

namespace Chip8 {

const glm::mat4 PROJECTION = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

Application::Application(const char* rom_location)
    : m_RomLocation(rom_location), m_Interpreter(rom_location) {}

Application::~Application() {}

bool Application::Initialize() {
  // Init SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    LOG_ERROR("Failed to initialize SDL: {}", SDL_GetError());

    return false;
  }
  LOG_INFO("SDL Initialized successfully.");

  // Init window
  const auto window_flags = SDL_WINDOW_OPENGL;
  const auto window_title =
      std::string("CHIP8: ") + std::filesystem::path(m_RomLocation).filename().string();
  m_Window = SDL_CreateWindow(window_title.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
  if (m_Window == nullptr) {
    LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());

    return false;
  }
  LOG_INFO("Window initialized successfully.");

  // Init OpenGL context
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
  SDL_GL_SetSwapInterval(0);

  // Init OpenGL loader (GLAD)
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

  // Init Audio
  m_AudioHandler = std::make_shared<AudioHandler>();

  SDL_ResumeAudioStreamDevice(m_AudioHandler->GetStream());
  m_AudioHandler->PauseStream();

  // Init ImGui
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

  // Init shader
  m_Shader = std::make_unique<Shader>();
  m_Shader->Load("src/Shaders/display.vert", "src/Shaders/display.frag");
  m_Shader->SetActive();
  m_Shader->SetUniformMat4(PROJECTION, "uProjection");

  // Init everything else
  m_Display = std::make_shared<Display>();
  m_Interpreter.SetDisplayPointer(m_Display);
  m_Interpreter.SetAudioPointer(m_AudioHandler);

  m_Display->UpdateDisplayData();

  m_TicksCount = SDL_GetTicks();

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
  const int time_cap = 1000 / m_OpsPerSecond;

  if (m_TicksElapsed > time_cap) {
    if (((m_StepThrough && m_AdvanceNextStep) || !m_StepThrough)) {
      m_Interpreter.Run();
      m_AdvanceNextStep = false;
    }

    m_TicksElapsed = 0;

  } else {
    m_TicksElapsed += SDL_GetTicks() - m_TicksCount;
  }

  m_TicksCount = SDL_GetTicks();
}

void Application::RenderState() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  this->RenderDebugUI();

  ImGui::Render();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  m_Display->RenderDisplay();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(m_Window);
}

void Application::RenderDebugUI() {
  ImGui::Begin("Settings");

  if (ImGui::Button("Restart")) {
    m_Interpreter.Restart(m_RomLocation);
  }

  ImGui::SliderInt("Speed (op/s)", &m_OpsPerSecond, 1, 1000);

  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Checkbox("Step through", &m_StepThrough);
    if (ImGui::Button("Next step")) {
      m_AdvanceNextStep = true;
    }
    m_Interpreter.DisplayDebugMenu();
  }
  ImGui::End();
}

}  // namespace Chip8
