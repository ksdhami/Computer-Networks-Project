#include <stdio.h>
#include <string>
#include <iostream>
#include "Player.h"

class GameLobby{
private:
  std::vector<Player> list;

public:
  GameLobby(Player player);
  bool isReady(std::string str);
};
