#include "Shader.h"

#include <SDL3/SDL_log.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "Logging.h"

Shader::Shader() : m_ShaderProgram(0), m_FragmentShader(0), m_VertexShader(0) {}

bool Shader::Load(const std::string &vertName, const std::string &fragName) {
  // Compile vertex and pixel shaders
  if (!CompileShader(vertName, GL_VERTEX_SHADER, m_VertexShader) ||
      !CompileShader(fragName, GL_FRAGMENT_SHADER, m_FragmentShader)) {
    return false;
  }

  // Now create a shader program that
  // links together the vertex/frag shaders
  m_ShaderProgram = glCreateProgram();
  glAttachShader(m_ShaderProgram, m_VertexShader);
  glAttachShader(m_ShaderProgram, m_FragmentShader);
  glLinkProgram(m_ShaderProgram);

  // Verify that the program linked successfully
  if (!IsValidProgram()) {
    return false;
  }

  return true;
}

bool Shader::CompileShader(const std::string &file_name, GLenum shader_type, GLuint &out_shader) {
  std::ifstream shaderFile(file_name);
  if (shaderFile.is_open()) {
    // Read all the text into a string
    std::stringstream sstream;
    sstream << shaderFile.rdbuf();
    std::string contents = sstream.str();
    const char *contentsChar = contents.c_str();

    // Create a shader of the specified type
    out_shader = glCreateShader(shader_type);
    // Set the source characters and try to compile
    glShaderSource(out_shader, 1, &(contentsChar), nullptr);
    glCompileShader(out_shader);

    if (!IsShaderCompiled(out_shader)) {
      LOG_ERROR("Failed to compile shader {}", file_name.c_str());
      return false;
    }
  } else {
    LOG_ERROR("Shader file not found: {}", file_name.c_str());
    return false;
  }

  return true;
}

bool Shader::IsShaderCompiled(GLuint shader) {
  GLint status;
  // Query the compile status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shader, 511, nullptr, buffer);
    LOG_ERROR("GLSL Compile failed: {}", buffer);
    return false;
  }

  return true;
}

bool Shader::IsValidProgram() {
  GLint status;
  // Query the link status
  glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    char buffer[512];
    memset(buffer, 0, 512);
    glGetProgramInfoLog(m_ShaderProgram, 511, nullptr, buffer);
    LOG_ERROR("GLSL Link Status: {}", buffer);
    return false;
  }

  return true;
}

void Shader::Unload() {
  // Delete the program/shaders
  glDeleteProgram(m_ShaderProgram);
  glDeleteShader(m_VertexShader);
  glDeleteShader(m_FragmentShader);
}

void Shader::SetActive() {
  // Set this program as the active one
  glUseProgram(m_ShaderProgram);
}

void Shader::SetUniformMat4(const glm::mat4 &mat, const std::string &name) {
  unsigned int uniform_location = glGetUniformLocation(m_ShaderProgram, name.c_str());

  glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformFloat(const float &input, const std::string &name) {
  unsigned int uniform_location = glGetUniformLocation(m_ShaderProgram, name.c_str());

  glUniform1f(uniform_location, input);
}

Shader::~Shader() {}
