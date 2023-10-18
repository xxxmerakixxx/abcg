#include "window.hpp"

void Window::onEvent(SDL_Event const &event) {
  // Keyboard even

  if (event.type == SDL_KEYDOWN) {
    
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {

    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
  }

  // // Mouse events
  // if (event.type == SDL_MOUSEBUTTONDOWN) {
    
  //   if (event.button.button == SDL_BUTTON_RIGHT)
  //     m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
  // }
  // if (event.type == SDL_MOUSEBUTTONUP) {
   
  //   if (event.button.button == SDL_BUTTON_RIGHT)
  //     m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
  // }
  // if (event.type == SDL_MOUSEMOTION) {
  //   glm::ivec2 mousePosition;
  //   SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  //   glm::vec2 direction{mousePosition.x - m_viewportSize.x / 2,
  //                       -(mousePosition.y - m_viewportSize.y / 2)};

  //   m_carp.m_rotation = std::atan2(direction.y, direction.x) - M_PI_2;
  // }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Create program to render the stars
  m_starsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "stars.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "stars.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0.117647059f, 0.564705882f, 1, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;

  m_starLayers.create(m_starsProgram, 25);
  m_carp.create(m_objectsProgram);
  m_fishes.create(m_objectsProgram, 3);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_carp.update(m_gameData, deltaTime);
  m_starLayers.update(m_carp, deltaTime);
  m_fishes.update(m_carp, deltaTime);

  
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_starLayers.paint();
  m_fishes.paint();
  m_carp.paint(m_gameData);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_objectsProgram);

  m_fishes.destroy();
  m_carp.destroy();
  m_starLayers.destroy();
}

void Window::checkCollisions() {
  // Check collision between ship and asteroids
  for (auto const &asteroid : m_fishes.m_fishes) {
    auto const asteroidTranslation{asteroid.m_translation};
    auto const distance{
        glm::distance(m_carp.m_translation, asteroidTranslation)};

  }

  // Check collision between bullets and asteroids
  // for (auto &bullet : m_bullets.m_bullets) {
  //   if (bullet.m_dead)
  //     continue;

  //   for (auto &asteroid : m_fishes.m_fishes) {
  //     for (auto const i : {-2, 0, 2}) {
  //       for (auto const j : {-2, 0, 2}) {
  //         auto const asteroidTranslation{asteroid.m_translation +
  //                                        glm::vec2(i, j)};
  //         auto const distance{
  //             glm::distance(bullet.m_translation, asteroidTranslation)};

  //         if (distance < m_bullets.m_scale + asteroid.m_scale * 0.85f) {
  //           asteroid.m_hit = true;
  //           bullet.m_dead = true;
  //         }
  //       }
  //     }
  //   }

    // Break asteroids marked as hit
  //   for (auto const &asteroid : m_fishes.m_fishes) {
  //     if (asteroid.m_hit && asteroid.m_scale > 0.10f) {
  //       std::uniform_real_distribution randomDist{-1.0f, 1.0f};
  //       std::generate_n(std::back_inserter(m_fishes.m_fishes), 3, [&]() {
  //         glm::vec2 const offset{randomDist(m_randomEngine),
  //                                randomDist(m_randomEngine)};
  //         auto const newScale{asteroid.m_scale * 0.5f};
  //         return m_fishes.makeAsteroid(
  //             asteroid.m_translation + offset * newScale, newScale);
  //       });
  //     }
  //   }

  //   m_fishes.m_fishes.remove_if([](auto const &a) { return a.m_hit; });
  // }
}

void Window::checkWinCondition() {
  if (m_fishes.m_fishes.empty()) {
    m_restartWaitTimer.restart();
  }
}