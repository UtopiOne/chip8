#include "Logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Chip8 {

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init() {
  spdlog::set_pattern("[%H:%M:%S] [%n] %v%$");

  s_Logger = spdlog::stderr_color_mt("CHIP8");
  s_Logger->set_level(spdlog::level::trace);

  spdlog::flush_on(spdlog::level::trace);
}

void Logger::OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                        GLsizei length, const GLchar* message,
                                        const void* userParam) {
  // annoying
  if (id == 131185) {
    return;
  }

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
      LOG_TRACE("[OPENGL] id: {} {}", message);
      break;
    }
    default: {
      LOG_INFO("[OPENGL] {}", message);
      break;
    }
  }
}

}  // namespace Chip8
