#include "Player.h"
#include <stdio.h>
#include <string>
#include <iostream>

Player::Player(std::string username) {this -> username = username;};
Player::Player(){this -> username = "";};
Player::~Player(){};
