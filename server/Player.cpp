#include <string>
// #include "Player.h"

using namespace std;


class Player
{
private:
    bool answered;
    bool votedKick;
    // string username;
    // string password;

public:
    string username;
    string password;
    Player(){}
    Player(string name, string pass)
    {
        setUsername(name);
        setPassword(pass);
        votedKick = false;
    }
    ~Player()
    {
    }

    // setters
    void setUsername(string name) { username = name; }
    void setPassword(string pass) { password = pass; }
    void setVoteKick(bool flag) { votedKick = flag; }

    // getters
    string getUsername() { return username; }
    string getPassword() { return password; }
    bool getVoteKick() { return votedKick; }
};
