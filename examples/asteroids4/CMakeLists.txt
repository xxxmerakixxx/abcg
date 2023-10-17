project(asteroids4)
add_executable(${PROJECT_NAME} main.cpp window.cpp fishes.cpp 
                               carp.cpp starlayers.cpp)
enable_abcg(${PROJECT_NAME})
