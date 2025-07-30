#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace Chip8 {

using PixelState = bool;
using PixelPos = unsigned int;
using Byte = uint8_t;
using Buffer = unsigned int;

constexpr unsigned int DISPLAY_WIDTH = 64;
constexpr unsigned int DISPLAY_HEIGHT = 32;

template <typename T>
struct Vector2 {
  T x, y;
};

class Display {
public:
  Display();

  void UpdateDisplayData();
  void RenderDisplay() const;

  void ClearDisplay();

  bool LoadSprite(const PixelPos x, const PixelPos y, std::vector<Byte>& sprite);

private:
  inline bool GetNthBit(Byte byte, int n) const;

private:
  std::array<std::array<PixelState, DISPLAY_HEIGHT>, DISPLAY_WIDTH> m_PixelData;

  Buffer m_VBO, m_VAO, m_EBO;
  unsigned int m_Size;
};

}  // namespace Chip8
