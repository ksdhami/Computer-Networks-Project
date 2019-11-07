#include <stdio.h>
#include <string>
#include <iostream>

class Player{
private:
  std::string username;
public:
  Player(std::string);
  Player();
  ~Player();
  void setName(std::string username) {this -> username = username;};
  std::string getName(){return username;};
};
