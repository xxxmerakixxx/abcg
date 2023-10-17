#include "carp.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Carp::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset ship attributes
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  std::array positions{
      // Carp body
      glm::vec2{-03.5f, +8.5f}, glm::vec2{+03.5f, +8.5f},
      glm::vec2{0.0f, -15.5f},
      glm::vec2{-02.5f, +12.5f},glm::vec2{+02.5f, +12.5f},
      glm::vec2{+0.0f, +12.5f},
      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array const indices{0, 1, 2,
                           0,3,4,
                           1,4,3,
                           0,5,1 
                           };
  // clang-format on

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
  abcg::glClearColor(0.117647059f, 0.564705882f, 1, 1);
}

void Carp::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing)
    return;

  // abcg::glClear(GL_COLOR_BUFFER_BIT);

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0)
    m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail for 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      // abcg::glEnable(GL_BLEND);
      // abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      abcg::glUniform4f(m_colorLoc, 50, 205, 50 , 0.0f);

      abcg::glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

      abcg::glDisable(GL_BLEND);
    }
  }

  // abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  // abcg::glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Carp::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Carp::update(GameData const &gameData, float deltaTime) {
  // Rotate
  if (gameData.m_input[gsl::narrow<size_t>(Input::Left)]){

    glm::vec2 direction{m_translation.x ,
                        -(m_translation.y)};
    m_rotation = std::atan2(direction.y, direction.x) * deltaTime + M_PI_2 ;
    if (m_translation.x > -0.9f)
      m_translation = glm:: vec2(m_translation.x - 1.5f * deltaTime, m_translation.y);
  }
    
  if (gameData.m_input[gsl::narrow<size_t>(Input::Right)]){
    glm::vec2 direction{m_translation.x,
                        -(m_translation.y)};
    m_rotation = std::atan2(direction.y, direction.x)* deltaTime - M_PI_2;
    if (m_translation.x < 0.9f)
      m_translation = glm:: vec2(m_translation.x + 1.5f * deltaTime, m_translation.y);
  }

  if (gameData.m_input[gsl::narrow<size_t>(Input::Up)]){
        glm::vec2 direction{m_translation.x,
                        -(m_translation.y)};
    m_rotation = std::atan2(direction.y, direction.x)* deltaTime + M_PI_2 * 4;
    if (m_translation.y < 0.9f) 
      m_translation = glm:: vec2(m_translation.x , m_translation.y + 1.5f * deltaTime);
  }
  
  if (gameData.m_input[gsl::narrow<size_t>(Input::Down)]) {
    glm::vec2 direction{m_translation.x,
                        - (m_translation.y)};
    m_rotation = std::atan2(-direction.y, direction.x)* deltaTime + M_PI_2 * 2;
    if (m_translation.y > -0.9f)
      m_translation = glm:: vec2(m_translation.x , m_translation.y - 1.5f * deltaTime);
  }


  // Apply thrust
  // if (gameData.m_input[gsl::narrow<size_t>(Input::Up)] &&
  //     gameData.m_state == State::Playing) {
  //   // Thrust in the forward vector
  //   auto const forward{glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation)};
  //   m_velocity += forward * deltaTime;
  // }
}

// void Carp::onEvent(SDL_Event const &event) {
//   // Keyboard events
//   if (event.type == SDL_KEYDOWN) {
//     // if (event.key.keysym.sym == SDLK_SPACE)
//     //   m_gameData.m_input.set(gsl::narrow<size_t>(Input::Fire));
//     if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
//       m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
//     if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
//       m_gameData.m_input.set(gsl::narrow<size_t>(Input::Down));
//     if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
//       m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
//     if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
//       m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
//   }
//   if (event.type == SDL_KEYUP) {
//     // if (event.key.keysym.sym == SDLK_SPACE)
//       // m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Fire));
//     if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
//       m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
//     if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
//       m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Down));
//     if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
//       m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
//     if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
//       m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
//   }

  // Mouse events
  // if (event.type == SDL_MOUSEBUTTONDOWN) {
  //   if (event.button.button == SDL_BUTTON_LEFT)
  //     m_gameData.m_input.set(gsl::narrow<size_t>(Input::Fire));
  //   if (event.button.button == SDL_BUTTON_RIGHT)
  //     m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
  // }
  // if (event.type == SDL_MOUSEBUTTONUP) {
  //   if (event.button.button == SDL_BUTTON_LEFT)
  //     m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Fire));
  //   if (event.button.button == SDL_BUTTON_RIGHT)
  //     m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
  // }
  // if (event.type == SDL_MOUSEMOTION) {
  //   glm::ivec2 mousePosition;
  //   SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    // glm::vec2 direction{mousePosition.x - m_viewportSize.x / 2,
    //                     -(mousePosition.y - m_viewportSize.y / 2)};

    // m_carp.m_rotation = std::atan2(direction.y, direction.x) - M_PI_2;
  // }
// }