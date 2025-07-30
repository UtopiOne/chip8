#pragma once

#include <glad/glad.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <string>

class Shader {
public:
  Shader();
  ~Shader();

  bool Load(const std::string &vertName, const std::string &fragName);

  unsigned int GetShaderID() const { return m_ShaderProgram; }

  void SetActive();

  void Unload();

  void SetUniformMat4(const glm::mat4 &mat, const std::string &name);
  void SetUniformFloat(const float &input, const std::string &name);

private:
  unsigned int m_VertexShader, m_FragmentShader;
  unsigned int m_ShaderProgram;

  bool IsShaderCompiled(GLuint shader) const;
  bool CompileShader(const std::string &file_name, GLenum shader_type, GLuint &out_shader);

  bool IsValidProgram() const;
};
