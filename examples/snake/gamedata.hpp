#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Up, Down, Left, Right };
enum class State { Playing };

struct GameData {
  State m_state{State::Playing};
  std::bitset<5> m_input;  // [fire, up, down, left, right]
};

#endif