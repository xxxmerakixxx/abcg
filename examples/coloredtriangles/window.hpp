#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  GLuint m_program{};

  std::default_random_engine m_randomEngine;

  std::array<glm::vec4, 3> m_colors{{{0.36f, 0.83f, 1.00f, 1},
                                     {0.63f, 0.00f, 0.61f, 1},
                                     {1.00f, 0.69f, 0.30f, 1}}};

  void setupModel();
};

#endif