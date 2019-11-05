#include <string>
// #include "Player.h"

using namespace std;

class Player
{
private:
    string username;
    string password;

public:
    Player(string name, string pass)
    {
        setUsername(name);
        setPassword(pass);
    }
    ~Player()
    {
    }

    // setters
    void setUsername(string name) { username = name; }
    void setPassword(string pass) { password = pass; }

    // getters
    string getUsername() { return username; }
    string getPassword() { return password; }
};

// Player::Player(string name, string pass)
// {
//     setUsername(name);
//     setPassword(pass);
// }

// Player::~Player()
// {

// }
