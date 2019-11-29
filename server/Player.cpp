#include <string>
// #include "Player.h"

using namespace std;

class Player
{
private:
    string addr;
    string port;
    string username;
    string password;
    bool voteKicked;

public:
    Player() {}
    Player(string name, string pass)
    {
        setUsername(name);
        setPassword(pass);
        voteKicked = false;
    }
    ~Player()
    {
    }

    // setters
    void setUsername(string name) { username = name; }
    void setPassword(string pass) { password = pass; }
    void setVoteKick(bool vote) { voteKicked = vote; }
    void setAddr(string ip) { addr = ip; }
    void setPort(string num) { port = num; }

    // getters
    string getUsername() { return username; }
    string getPassword() { return password; }
    bool getVoteKick() { return voteKicked; }
    string getAddr() { return addr; }
    string getPort() { return port; }
};
