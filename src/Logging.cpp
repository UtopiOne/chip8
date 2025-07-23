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

}  // namespace Chip8
