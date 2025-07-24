#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Chip8 {

class Logger {
public:
  static void Init();
  static void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         GLsizei length, const GLchar* message,
                                         const void* userParam);

  inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
  static std::shared_ptr<spdlog::logger> s_Logger;
};

#if LOGGING_ENABLED

#define LOG_TRACE(...) Chip8::Logger::GetLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...) Chip8::Logger::GetLogger()->info(__VA_ARGS__);
#define LOG_WARN(...) Chip8::Logger::GetLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...) Chip8::Logger::GetLogger()->critical(__VA_ARGS__);

#else

#define LOG_TRACE(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)

#endif

}  // namespace Chip8
