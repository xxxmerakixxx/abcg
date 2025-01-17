#include "fishes.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Fishes::create(GLuint program, int quantity) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_fishes.clear();
  m_fishes.resize(quantity);

  for (auto &fish : m_fishes) {
    fish = makeFish();

    // Make sure the asteroid won't collide with the ship
    do {
      fish.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(fish.m_translation) < 0.5f);
  }
}

void Fishes::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &fish : m_fishes) {
    abcg::glBindVertexArray(fish.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &fish.m_color.r);
    abcg::glUniform1f(m_scaleLoc, fish.m_scale);
    abcg::glUniform1f(m_rotationLoc, fish.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) { 
        abcg::glUniform2f(m_translationLoc, fish.m_translation.x + j,
                          fish.m_translation.y + i);

        abcg::glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
      }
    }
    abcg::glBindVertexArray(0);
  }
  
  abcg::glUseProgram(0);
}

void Fishes::destroy() {
  for (auto &fish : m_fishes) {
    abcg::glDeleteBuffers(1, &fish.m_VBO);
    abcg::glDeleteBuffers(1, &fish.m_EBO);
    abcg::glDeleteVertexArrays(1, &fish.m_VAO);
  }
}

void Fishes::update(const Carp &carp, float deltaTime) {
  for (auto &fish : m_fishes) {
    fish.m_translation -= carp.m_velocity * deltaTime;
    fish.m_rotation = glm::wrapAngle(
        fish.m_rotation + fish.m_angularVelocity * deltaTime);
    fish.m_translation += fish.m_velocity * deltaTime;

    // Wrap-around
    if (fish.m_translation.x < -1.0f)
      fish.m_translation.x += 2.0f;
    if (fish.m_translation.x > +1.0f)
      fish.m_translation.x -= 2.0f;
    if (fish.m_translation.y < -1.0f)
      fish.m_translation.y += 2.0f;
    if (fish.m_translation.y > +1.0f)
      fish.m_translation.y -= 2.0f;
  }
}

Fishes::Fish Fishes::makeFish(glm::vec2 translation,
                                            float scale) {
  Fish fish;

  auto &re{m_randomEngine}; // Shortcut

  
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

  // Get a random color (actually, a grayscale)
  std::uniform_real_distribution randomIntensity(0.1f, 1.0f);
  fish.m_color = glm::vec4(randomIntensity(re));

  fish.m_color.a = 1.0f;
  fish.m_color.b = 0.0f;
  fish.m_color.g = 0.0f;
  fish.m_rotation = 0.0f;
  fish.m_scale = scale;
  fish.m_translation = translation;

  // Get a random angular velocity
  fish.m_angularVelocity = m_randomDist(re);

  // Get a random direction
  glm::vec2 const direction{m_randomDist(re), m_randomDist(re)};
  fish.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry data
  // std::vector<glm::vec2> positions{{0, 0}};
  // auto const step{M_PI * 2 / asteroid.m_polygonSides};
  // std::uniform_real_distribution randomRadius(0.8f, 1.0f);
  // for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
  //   auto const radius{randomRadius(re)};
  //   positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  // }
  // positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &fish.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, fish.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // /  Generate EBO
  abcg::glGenBuffers(1, &fish.m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fish.m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &fish.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(fish.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, fish.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fish.m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
  
  return fish;
}