#include "GameLobby.h"
#include <stdio.h>
#include <string>
#include <iostream>

GameLobby::GameLobby(Player player) {
  list.push_back(player);

  std::cout << list[0].getName() + "\n";
};

bool isReady(std::string str){
  if (str == "/ready") {
    return 1;
  } else {return 0;};
};
