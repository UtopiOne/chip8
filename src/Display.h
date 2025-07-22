#pragma once

#include <array>

namespace Chip8 {

using PixelState = bool;
using Buffer = unsigned int;

constexpr unsigned int DISPLAY_WIDTH = 64;
constexpr unsigned int DISPLAY_HEIGHT = 32;

class Display {
public:
  Display();

  void RenderDisplay();

  PixelState GetPixelState(int x, int y) { return m_PixelData[x][y]; }
  void SetPixelState(PixelState state, int x, int y) {
    m_PixelData[x][y] = state;
  }

private:
  std::array<std::array<PixelState, DISPLAY_HEIGHT>, DISPLAY_WIDTH> m_PixelData;

  Buffer m_VBO, m_VAO, m_EBO;
};

} // namespace Chip8
