#include "Display.h"
#include "Application.h"

#include <cstdint>
#include <glad/glad.h>

#include <vector>

namespace Chip8 {

Display::Display() {
  constexpr auto pixel_width = 2.0 / WINDOW_WIDTH * DISPLAY_WIDTH;
  constexpr auto pixel_height = 2.0 / WINDOW_HEIGHT * DISPLAY_HEIGHT;

  // std::vector<float> positions;
  // std::vector<unsigned int> elements;
  //
  // uint16_t cnt = 0;
  // for (int x = 0; x < DISPLAY_WIDTH; ++x) {
  //   for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
  //     if (GetPixelState(x, y)) {
  //       positions.push_back(-1.0);
  //     }
  //   }
  // }
  //
  float positions[] = {
      -1.0, // 0
      1.0,
      -1.0, // 1
      1.0f - pixel_height,
      -1.0 + pixel_width, // 2
      1.0f,
      -1.0 + pixel_width, // 3
      1.0f - pixel_height,
  };

  unsigned int elements[] = {0, 1, 2, 3, 2, 1};

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), (void *)&positions,
               GL_STATIC_DRAW);

  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

void Display::RenderDisplay() {
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
} // namespace Chip8
