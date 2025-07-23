#include "Display.h"

#include <glad/glad.h>

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

  std::vector<Vector2> positions;
  std::vector<unsigned int> elements;

  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    m_PixelData[x][0] = true;
  }
  for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
    m_PixelData[0][y] = true;
  }

  uint16_t cnt = 0;
  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
      if (this->GetPixelState(x, y)) {
        auto bottom_left = Vector2{-1.0 + x * PIXEL_WIDTH, -1.0 + y * PIXEL_HEIGHT};
        auto top_left = Vector2{-1.0 + x * PIXEL_WIDTH, -1.0 + (y + 1) * PIXEL_HEIGHT};
        auto bottom_right = Vector2{-1.0 + (x + 1) * PIXEL_WIDTH, -1.0 + y * PIXEL_HEIGHT};
        auto top_right = Vector2{-1.0 + (x + 1) * PIXEL_WIDTH, -1.0 + (y + 1) * PIXEL_HEIGHT};

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

  m_Size = elements.size();

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(*positions.data()) * positions.size(), positions.data(),
               GL_STATIC_DRAW);

  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*elements.data()) * elements.size(), elements.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
  glEnableVertexAttribArray(0);
}

void Display::RenderDisplay() {
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_Size, GL_UNSIGNED_INT, 0);
}
}  // namespace Chip8
