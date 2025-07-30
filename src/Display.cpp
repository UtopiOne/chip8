#include "Display.h"

#include <fmt/base.h>
#include <glad/glad.h>

#include <cstdint>
#include <vector>

#include "Logging.h"

namespace Chip8 {

constexpr auto PIXEL_WIDTH = 2.0 / DISPLAY_WIDTH;
constexpr auto PIXEL_HEIGHT = 2.0 / DISPLAY_HEIGHT;

Display::Display() {
  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
      m_PixelData[x][y] = false;
    }
  }

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
  glEnableVertexAttribArray(0);
}

void Display::UpdateDisplayData() {
  int pixel_count = 0;
  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
      if (m_PixelData[x][y]) {
        pixel_count++;
      }
    }
  }

  LOG_TRACE("Pixel Count: {}", pixel_count);

  std::vector<Vector2<double>> positions;
  std::vector<unsigned int> elements;

  positions.reserve(pixel_count * 4);
  elements.reserve(pixel_count * 6);

  uint16_t cnt = 0;
  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
      if (m_PixelData[x][y]) {
        Vector2<double> bottom_left{-1.0 + x * PIXEL_WIDTH, 1.0 - (y + 1) * PIXEL_HEIGHT};
        Vector2<double> top_left{-1.0 + x * PIXEL_WIDTH, 1.0 - y * PIXEL_HEIGHT};
        Vector2<double> bottom_right{-1.0 + (x + 1) * PIXEL_WIDTH, 1.0 - (y + 1) * PIXEL_HEIGHT};
        Vector2<double> top_right{-1.0 + (x + 1) * PIXEL_WIDTH, 1.0 - y * PIXEL_HEIGHT};

        positions.push_back(bottom_left);
        positions.push_back(top_left);
        positions.push_back(bottom_right);
        positions.push_back(top_right);

        elements.push_back(0 + 4 * cnt);
        elements.push_back(1 + 4 * cnt);
        elements.push_back(2 + 4 * cnt);
        elements.push_back(3 + 4 * cnt);
        elements.push_back(2 + 4 * cnt);
        elements.push_back(1 + 4 * cnt);

        cnt += 1;
      }
    }
  }

  LOG_INFO("Updating display...");

  m_Size = elements.size();

  glBufferData(GL_ARRAY_BUFFER, sizeof(*positions.data()) * positions.size(), positions.data(),
               GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*elements.data()) * elements.size(), elements.data(),
               GL_STATIC_DRAW);
}

void Display::RenderDisplay() const {
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_Size, GL_UNSIGNED_INT, 0);
}

void Display::ClearDisplay() {
  for (auto x = 0; x < DISPLAY_WIDTH; ++x) {
    for (auto y = 0; y < DISPLAY_HEIGHT; ++y) {
      m_PixelData[x][y] = false;
    }
  }

  this->UpdateDisplayData();
}

bool Display::LoadSprite(const PixelPos x, const PixelPos y, std::vector<Byte>& sprite) {
  bool flag = false;

  const Vector2<PixelPos> starting_pos{x % DISPLAY_WIDTH, y % DISPLAY_HEIGHT};
  const Vector2<PixelPos> end_pos{
      (starting_pos.x + 8) >= DISPLAY_WIDTH ? DISPLAY_WIDTH : starting_pos.x + 8,
      static_cast<PixelPos>((starting_pos.y + sprite.size()) >= DISPLAY_HEIGHT
                                ? DISPLAY_HEIGHT
                                : starting_pos.y + sprite.size())};

  for (auto x = starting_pos.x; x < end_pos.x; ++x) {
    for (auto y = starting_pos.y; y < end_pos.y; ++y) {
      PixelState value = GetNthBit(sprite[y - starting_pos.y], x - starting_pos.x);

      if (value) {
        m_PixelData[x][y] = !m_PixelData[x][y];

        if (!m_PixelData[x][y]) {
          flag = true;
        }
      }
    }
  }

  return flag;
}

bool Display::GetNthBit(Byte byte, int n) const {
  Byte mask = 128 >> n;

  if (byte & mask) {
    return true;
  }

  return false;
}

}  // namespace Chip8
