/*
 * A simple TCP select server that accepts multiple connections and echo message back to the clients
 * For use in CPSC 441 lectures
 * Instructor: Prof. Mea Wang
 */

#include <iostream>
#include <sys/socket.h> // for socket(), connect(), send(), and recv()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <utility>
#include "Quiz.cpp"
#include <time.h>  // for timer

#define QUESTIONFILE "qq.txt"
// #define QUESTIONFILE "questions.txt"
#define LEADERBOARDFILE "leaderboard.txt"
#define NUMPLAYERS 3

using namespace std;

const int BUFFERSIZE = 512; // Size the message buffers
const int MAXPENDING = 10;  // Maximum pending connections

fd_set recvSockSet; // The set of descriptors for incoming connections
int maxDesc = 0;    // The max descriptor
bool terminated = false;
bool done = false;
char inBuffer[BUFFERSIZE]; // Buffer for the message from the server

int serverSockTCP;                // server socket descriptor
int clientSock;                   // client socket descriptor
struct sockaddr_in clientAddr;    // address of the client
struct timeval timeout = {0, 10}; // The timeout value for select()
struct timeval selectTime;
fd_set tempRecvSockSet; // Temp. receive socket set for select()

bool unknownCommandFlag = false;
string unknownCommand;

Quiz quiz;                             // quiz game object
bool quizStarted = false;              // game started
bool readQuestionsFromFile = false;    // question file read in object made
bool playerReady = false;              // player ready to play game
bool requestQuestionRepeat = false;    // client requested question again
bool questionSent = false;             // client got question in game
bool answerRec = false;                // server received answers from participants
bool gameOver = false;                 // game is over
vector<Player> players;                // players in server; both in game and not in game
vector<pair<string, int> > leaderboard; // key(name):value(score) pair for leaderboard
bool seeLeaderboard = false;           // user requests to see leaderboard
bool leaderboardLoaded = false;
bool seePlayers = false;
bool chattingTime = true;       // not used yet
bool answeringTime = false;     // not used yet
bool earlyAnswer = false;
bool wrTimerDone = false;   // waiting room timer
string action = "";  // for timer print statement
//bool showTimer = false;
//int numTimeouts = 0;
bool serverMessage = false;

bool chatMessage = false;
bool singlePlayerOnly = false;

char type;
string recMsg;
bool noSend = false;
bool logoutRequest = false;
bool missingEventHandler = false;

time_t start, countdown, previousSecond, current, timeLeft; // for timer


vector<int> socks;
vector<struct sockaddr_in> clients;

void initServerTCP(int &, int port);
void processSockets(fd_set);
void sendDataTCP(int, char[], int);
void receiveDataTCP(int, char[], int &);

void readQuestionFile();
void endGameCleanup(int sock, char *buffer, int size, int bytesSent);
pair<string, int> gameMessage();
void loadLeaderboard();
void updateLeaderboard(int score, string name);
bool sortPairSecond(const pair<string,int> &a, const pair<string,int> &b);
void setupTimer(int);

// https://stackoverflow.com/questions/15063301/how-can-i-find-an-object-in-a-vector-based-on-class-properties
class FindAttribute
{
    string name_;

public:
    FindAttribute(const string &name)
        : name_(name)
    {
    }

    bool operator()(Player &attr)
    {
        return attr.getPassword() == name_;
    }
};

int main(int argc, char *argv[])
{

    // Check for input errors
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <Listening Port>" << endl;
        exit(1);
    }

    // Initilize the server
    initServerTCP(serverSockTCP, atoi(argv[1]));

    // Clear the socket sets
    FD_ZERO(&recvSockSet);

    // Add the listening socket to the set
    FD_SET(serverSockTCP, &recvSockSet);
    maxDesc = max(maxDesc, serverSockTCP);

    socklen_t size = sizeof(clientAddr);

    bool TCPInc = false;

    // Run the server until a "terminate" command is received)
    while (!terminated)
    {
        // read file containing questions
        if (!readQuestionsFromFile)
        {
            readQuestionFile();
            readQuestionsFromFile = true;
        }

        if (!leaderboardLoaded)
        {
            cout << "Leaderboard Loading\n" << endl;
            loadLeaderboard();
        }

        if (playerReady && !wrTimerDone) {
            setupTimer(15);
            while (timeLeft > 0) {
                current = time(0);
                timeLeft = countdown - current;
                if (timeLeft != previousSecond && (timeLeft <= 5 || timeLeft == 10)) {
                     cout << timeLeft << " seconds left!" << endl;  // print to server
                     previousSecond = timeLeft;
//                     showTimer = true;  // not working
                }

                // copy the receive descriptors to the working set
                 memcpy(&tempRecvSockSet, &recvSockSet, sizeof(recvSockSet));

                // Select timeout has to be reset every time before select() is
                // called, since select() may update the timeout parameter to
                // indicate how much time was left.
                selectTime = timeout;
                int ready = select(maxDesc + 1, &tempRecvSockSet, NULL, NULL, &selectTime);
                if (ready < 0)
                {
                    cout << "select() failed" << endl;
                    break;
                }

                TCPInc = false;

                // First, process new connection request, if any.
                if (FD_ISSET(serverSockTCP, &tempRecvSockSet))
                {
                    //cout << "found a tcp client" <<endl;
                    // set the size of the client address structure

                    // Establish a connection
                    if ((clientSock = accept(serverSockTCP, (struct sockaddr *)&clientAddr, &size)) < 0)
                    break;
                    cout << "Accepted a connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << endl;

                    clients.push_back(clientAddr);
                    socks.push_back(clientSock);

                    Player player(inet_ntoa(clientAddr.sin_addr), to_string(clientAddr.sin_port));
                    players.push_back(player);
                    cout << "Player added to server vector: " << players.back().getUsername() << " with password: " << players.back().getPassword() << endl;

                    // Add the new connection to the receive socket set
                    FD_SET(clientSock, &recvSockSet);
                    maxDesc = max(maxDesc, clientSock);
                    TCPInc = true;
                }
                else
                {
                    processSockets(tempRecvSockSet);
                }

                // Then process messages waiting at each ready socket

                if (!~(TCPInc))
                {
                    processSockets(tempRecvSockSet);
                }

                TCPInc = false;

            }
            wrTimerDone = true;
            serverMessage = true;
            //quizStarted = true;
            processSockets(tempRecvSockSet);
        }

        if (quizStarted) {
            if (chattingTime)
                setupTimer(30);
            else if (answeringTime)
                setupTimer(5);

            while (timeLeft > 0) {
                current = time(0);
                timeLeft = countdown - current;
                if (timeLeft != previousSecond) {
                    cout << timeLeft << " seconds left!" << endl;
                    previousSecond = timeLeft;
////
                    }
                          // copy the receive descriptors to the working set
                        memcpy(&tempRecvSockSet, &recvSockSet, sizeof(recvSockSet));

                        // Select timeout has to be reset every time before select() is
                        // called, since select() may update the timeout parameter to
                        // indicate how much time was left.
                        selectTime = timeout;
                        int ready = select(maxDesc + 1, &tempRecvSockSet, NULL, NULL, &selectTime);
                        if (ready < 0)
                        {
                            cout << "select() failed" << endl;
                            break;
                        }

                        TCPInc = false;

                        // First, process new connection request, if any.
                        if (FD_ISSET(serverSockTCP, &tempRecvSockSet))
                        {
                            //cout << "found a tcp client" <<endl;
                            // set the size of the client address structure

                            // Establish a connection
                            if ((clientSock = accept(serverSockTCP, (struct sockaddr *)&clientAddr, &size)) < 0)
                                break;
                             cout << "Accepted a connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << endl;

                            clients.push_back(clientAddr);
                            socks.push_back(clientSock);
							/*
							cout << "current clients: ";
							for(auto c : clients){
								cout << inet_ntoa(c.sin_addr);
							}
							cout << endl;
							*/
                            Player player(inet_ntoa(clientAddr.sin_addr), to_string(clientAddr.sin_port));
                            players.push_back(player);
                            cout << "Player added to server vector: " << players.back().getUsername() << " with password: " << players.back().getPassword() << endl;

                            // Add the new connection to the receive socket set
                            FD_SET(clientSock, &recvSockSet);
                            maxDesc = max(maxDesc, clientSock);
                            TCPInc = true;
                        }
                        else
                        {
                            processSockets(tempRecvSockSet);
                        }

                        // Then process messages waiting at each ready socket

                        if (!~(TCPInc))
                        {
                            processSockets(tempRecvSockSet);
                        }

                        TCPInc = false;
            } // inner while loop
                if (chattingTime) {
                    if (earlyAnswer)
                        earlyAnswer = false;
                    else {
                        chattingTime = false;
                        answeringTime = true;
                    }
                }
                else if (answeringTime) {
                    chattingTime = true;
                    answeringTime = false;
                    if (!earlyAnswer) {   // Time ran out without answering
                        answerRec = true;
                        questionSent = false;
                        quiz.addResponse('t');  // CHANGE
                        //++numTimeouts;
                     // TODO: Go to next question
                  }
                    else
                        earlyAnswer = false;
                }
        } // if quizStarted statement
        else {
            // copy the receive descriptors to the working set
             memcpy(&tempRecvSockSet, &recvSockSet, sizeof(recvSockSet));

            // Select timeout has to be reset every time before select() is
            // called, since select() may update the timeout parameter to
            // indicate how much time was left.
            selectTime = timeout;
            int ready = select(maxDesc + 1, &tempRecvSockSet, NULL, NULL, &selectTime);
            if (ready < 0)
            {
                cout << "select() failed" << endl;
                break;
            }

            TCPInc = false;

            // First, process new connection request, if any.
            if (FD_ISSET(serverSockTCP, &tempRecvSockSet))
            {
                //cout << "found a tcp client" <<endl;
                // set the size of the client address structure

                // Establish a connection
                if ((clientSock = accept(serverSockTCP, (struct sockaddr *)&clientAddr, &size)) < 0)
                break;
                cout << "Accepted a connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << endl;

                clients.push_back(clientAddr);
                socks.push_back(clientSock);

                Player player(inet_ntoa(clientAddr.sin_addr), to_string(clientAddr.sin_port));
                players.push_back(player);
                cout << "Player added to server vector: " << players.back().getUsername() << " with password: " << players.back().getPassword() << endl;

                // Add the new connection to the receive socket set
                FD_SET(clientSock, &recvSockSet);
                maxDesc = max(maxDesc, clientSock);
                TCPInc = true;
            }
            else
            {
                processSockets(tempRecvSockSet);
            }

            // Then process messages waiting at each ready socket

            if (!~(TCPInc))
            {
                processSockets(tempRecvSockSet);
            }

            TCPInc = false;

        } // else statement

    } // outer while loop

    // Close the connections with the client
    for (int sock = 0; sock <= maxDesc; sock++)
    {
        if (FD_ISSET(sock, &recvSockSet))
            close(sock);
    }

    // Close the server sockets
    close(serverSockTCP);
}


void readQuestionFile()
{
    string line;      // line from file
    string qQuestion; // question to add
    string qChoiceA;  // choice A to add
    string qChoiceB;  // choice B to add
    string qChoiceC;  // choice C to add
    string qChoiceD;  // choice D to add
    char qAnswer;     // answer to add

    ifstream fin(QUESTIONFILE, ios::in);

    // file not found error
    if (!fin)
    {
        cerr << "error: open file for input failed!" << endl;
        return;
    }

    while (getline(fin, line))
    {
        // check first character of file to determine what it is
        // could be more efficient way to format file and do this step; also means changing question class
        switch (line[0])
        {
        case '?':
            qQuestion = line.substr(1);
            // cout << "Line: " << line << " Question: " << qQuestion << endl;
            break;
        case '1':
            qChoiceA = line.substr(1);
            // cout << "Line: " << line << " Option 1: " << qChoiceA << endl;
            break;
        case '2':
            qChoiceB = line.substr(1);
            // cout << "Line: " << line << " Option 2: " << qChoiceB << endl;
            break;
        case '3':
            qChoiceC = line.substr(1);
            // cout << "Line: " << line << " Option 3: " << qChoiceC << endl;
            break;
        case '4':
            qChoiceD = line.substr(1);
            // cout << "Line: " << line << " Option 4: " << qChoiceD << endl;
            break;
        case '!':
            qAnswer = line.back();
            // cout << "Line: " << line << " Answer: " << qAnswer << endl;
            break;
        default:
            // cerr << "New Question\n" << endl;
            // create question object
            Question ques(qQuestion, qChoiceA, qChoiceB, qChoiceC, qChoiceD, qAnswer);
            // cout << "Repeat Question Command Check \n" << ques.getQuestionsAndChoicesString() << endl;
            quiz.addQuestion(ques); // add question to quiz set
            break;
        }
    }

    fin.close();
}

void initServerTCP(int &serverSock, int port)
{
    struct sockaddr_in serverAddr; // address of the server

    // Create a TCP socket
    // * AF_INET: using address family "Internet Protocol address"
    // * SOCK_STREAM: Provides sequenced, reliable, bidirectional, connection-mode byte streams.
    // * IPPROTO_TCP: TCP protocol
    if ((serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        cout << "socket() failed" << endl;
        exit(1);
    }

    // Free up the port before binding
    // * sock: the socket just created
    // * SOL_SOCKET: set the protocol level at the socket level
    // * SO_REUSEADDR: allow reuse of local addresses
    // * &yes: set SO_REUSEADDR on a socket to true (1)
    // * sizeof(int): size of the value pointed by "yes"
    int yes = 1;
    if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        cout << "setsockopt() failed" << endl;
        exit(1);
    }

    // Initialize the server information
    // Note that we can't choose a port less than 1023 if we are not privileged users (root)
    memset(&serverAddr, 0, sizeof(serverAddr));     // Zero out the structure
    serverAddr.sin_family = AF_INET;                // Use Internet address family
    serverAddr.sin_port = htons(port);              // Server port number
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface

    // Bind to the local address
    if (bind(serverSock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "bind() failed" << endl;
        exit(1);
    }

    // Listen for connection requests
    if (listen(serverSock, MAXPENDING) < 0)
    {
        cout << "listen() failed" << endl;
        exit(1);
    }
}

void processSockets(fd_set readySocks)
{
    char *buffer = new char[BUFFERSIZE]; // Buffer for the message from the server
    int size;                            // Actual size of the message

    // Loop through the descriptors and process
    for (int sock = 0; sock <= maxDesc; sock++)
    {
        if (!FD_ISSET(sock, &readySocks))
            continue;

        int length = sizeof(int);
        int type;

        getsockopt(sock, SOL_SOCKET, SO_TYPE, &type, (socklen_t *)&length);
        if (type == SOCK_STREAM)
        {
            // Clear the buffers
            memset(buffer, 0, BUFFERSIZE);
            //cout << "tcp buffs cleared" << endl;
            // Receive data from the client
            if (!serverMessage)
            {
                receiveDataTCP(sock, buffer, size);
            }
            //cout << "tcp receive done" << endl;
            // Echo the message back to the client
            sendDataTCP(sock, buffer, size);
            //cout << "tcp send done" << endl;
        }
        else
        {
            cout << "unknown socket type" << endl;
        }
    }

    delete[] buffer;
}

void receiveDataTCP(int sock, char *inBuffer, int &size)
{
    // Receive the message from client
    size = recv(sock, (char *)inBuffer, BUFFERSIZE, 0);

    // Check for connection close (0) or errors (< 0)
    if (size <= 0)
    {
        cout << "recv() failed, or the connection is closed. " << endl;
        FD_CLR(sock, &recvSockSet);

        // Update the max descriptor
        while (FD_ISSET(maxDesc, &recvSockSet) == false)
            maxDesc -= 1;
        return;
    }

    recMsg = string(inBuffer);
    cout << "Message from TCP Client: " << recMsg;

    std::vector<int>::iterator it = find(socks.begin(), socks.end(), sock);

    int clientNum = std::distance(socks.begin(), it);
	cout << "received from client " << clientNum << endl;
    type = recMsg.at(0);
    if (type == 'm')
    {
        chatMessage = true;
        singlePlayerOnly = false;
        // non-command message
    }
    else if (type == 'r' && playerReady == false)
    {
        chatMessage = false;
        singlePlayerOnly = false;
        auto attr_iter = find_if(players.begin(), players.end(), FindAttribute(to_string(clientAddr.sin_port)));
        if (attr_iter != players.end())
        {
            cout << "player found with password: " << to_string(clientAddr.sin_port) << endl;
            auto index = distance(players.begin(), attr_iter);
            cout << "index of player is: " << index << endl;
            quiz.addPlayer(players.at(index));
            if (quiz.getTeam().size() == NUMPLAYERS)
            {
                playerReady = true;
            }
        }
    }
    else if (type == 'l')
    {
        chatMessage = false;
        singlePlayerOnly = true;
        if(!seeLeaderboard)
        {
            seeLeaderboard = true;
        }
    }
    else if (type == 'p')
    {
        chatMessage = false;
        singlePlayerOnly = true;
        if(!playerReady)
        {
            seePlayers = true;
        }
    }
    else if (type == 'h')
    {
        noSend = true;
        return;
    }
    else if (type == 'e')
    {
        chatMessage = false;
        // extra time
    }
    else if (type == 'k')
    {
        chatMessage = false;
        // kick
    }
    else if (type == 'q')
    {
        chatMessage = false;
        singlePlayerOnly = true;
        requestQuestionRepeat = true;
    }
    else if (type == 'a' && quizStarted && questionSent && recMsg.length()>=9)
    {
        chatMessage = false;
        singlePlayerOnly = false;
 //       if (answeringTime) {
            cout << "client replied with: " << recMsg << endl;
            cout << "client choice was: " << recMsg[9] << endl;
            quiz.addResponse(tolower(recMsg[9]));
            // TODO: change so wait for all teams response or timer expire
            if(quiz.getResponses().size() == quiz.getTeam().size())
            {
                if (chattingTime || (answeringTime && timeLeft > 0))
                    earlyAnswer = true;
                answerRec = true;
                timeLeft = 0;
            }
  //      }
  //      else
  //      {
  //          cout << "you cannot answer yet" << endl;
  //      }
    }
    else if (type == 'v')
    {
        chatMessage = false;
        singlePlayerOnly = true;
        // leave
    }
    else if (type == 'o')
    {
        chatMessage = false;
        singlePlayerOnly = false;
        cout << "User " << inet_ntoa(clients[clientNum].sin_addr) << ":" << clients[clientNum].sin_port << " has left" << endl;
        return;
        //close(sock);
    }
    else
    {
        chatMessage = false;
        singlePlayerOnly = false;
        cout << "missing event handler" << endl;
        missingEventHandler = true;
    }

    recMsg = recMsg.substr(1, recMsg.length() - 1);

    cout << "User " << inet_ntoa(clients[clientNum].sin_addr) << ":" << clients[clientNum].sin_port << ": " << recMsg;
}

void sendDataTCP(int sock, char *buffer, int size)
{
    int bytesSent = 0; // Number of bytes sent
    pair<string, int> message = gameMessage();
    string msg = message.first;
    cout << "message to send: " << msg << endl;
    int msgSize = message.second;

    std::vector<int>::iterator it = find(socks.begin(), socks.end(), sock);

    int clientNum = std::distance(socks.begin(), it);

    if (gameOver)
    {
        endGameCleanup(sock, buffer, size, bytesSent);
    }
    else if (missingEventHandler)
    {
        string msg = "Unknown command: " + unknownCommand;
        int size = msg.length();
        bytesSent = send(sock, (char *)msg.c_str(), size, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
        //cout << "SENT: " << bytesSent << endl;
        missingEventHandler = false;
    }
    else if (chatMessage)
    {
        if (type == 'o')
        {
            msg = "has left\n";
        }
        if (!answeringTime)
        {
            const char *tempAddr = inet_ntoa(clients[clientNum].sin_addr);
            string temp2 = string(tempAddr);
            string temp = "User ";
            temp += temp2;
            temp += ":";
            temp += to_string(clients[clientNum].sin_port);
            temp += ": ";
            temp += recMsg;
            size = temp.length();
            for (int asock : socks)
            {
                //cout << "got in" << endl;
                bytesSent = send(asock, (char *)temp.c_str(), size, 0);
                if (bytesSent < 0)
                {
                    cout << "error in sending: sendDataTCP: " << errno << endl;
                }
            }
        }
        if (type == 'o')
        {
            msg = "done";
            bytesSent = send(sock, (char *)recMsg.c_str(), size, 0);
            close(sock);
            FD_CLR(sock, &recvSockSet);
            clients.erase(clients.begin() + clientNum);
            socks.erase(socks.begin() + clientNum);
        }
        chatMessage = false;
        return;
    }
    else if (!chatMessage)
    {
        if (singlePlayerOnly) {
            bytesSent = send(sock, (char *)msg.c_str(), msgSize, 0);
            cout << to_string(bytesSent) << endl;
            if (bytesSent < 0)
            {
                cout << "error in sending: sendDataTCP" << endl;
                return;
            }
        }
        else {
            for (int asock : socks)
            {
                bytesSent = send(asock, (char *)msg.c_str(), msgSize, 0);
                // cout << to_string(bytesSent) << endl;
                if (bytesSent < 0)
                {
                    cout << "error in sending: sendDataTCP" << endl;
                    return;
                }
            }
        }
        return;
    }
    else if (noSend)
    {
        noSend = false;
        return;
    }
/*
    if (showTimer) // doesn't work
    {
        msg += "\n" + to_string(timeLeft) + " seconds left!\n";
        int sizeMsg = msg.length();
        bytesSent = send(sock, (char *)msg.c_str(), sizeMsg, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
        }
        showTimer = false;
    }
*/
    else // send message to user
    {
        // bytesSent = send(sock, (char *)msg.c_str(), msgSize, 0);
        // cout << to_string(bytesSent) << endl;
        // if (bytesSent < 0)
        // {
        //     cout << "error in sending: sendDataTCP" << endl;
        //     return;
        // }
    }
}

void endGameCleanup(int sock, char *buffer, int size, int bytesSent)
{
    cout << "END GAME\n" << endl;
    string msg = "GAME OVER\n";
    // TODO: print leaderboard
    msg += "Points: " + to_string(quiz.getPoints()) + " Fails: " + to_string(quiz.getFailCounter()) + "\n";

    updateLeaderboard(quiz.getPoints(), quiz.getTeam().front().getUsername());

    msg += "\nLeaderboard\n";

    int index = 10;

    if (leaderboard.size() < 10)
    {
        index = leaderboard.size();
    }

    for (int i = 0; i < index; i++)
    {
        msg += to_string(i+1);
        msg +=  ") ";
        msg += leaderboard.at(i).first;
        msg += " - ";
        msg += to_string(leaderboard.at(i).second);
        msg += "\n";
    }

    msg += "\n";
    msg += "Welcome Back To The Waiting Room";
    msg += "\n";
    msg += "Please enter a message to be sent to the server: ";
    msg += "\n";

    int sizeMsg = msg.length();

    for (int asock : socks)
    {
        bytesSent = send(asock, (char *)msg.c_str(), sizeMsg, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
    }

    quiz.clearTeam();
    readQuestionsFromFile = false;
    quizStarted = false;
    playerReady = false;
    gameOver = false;
    wrTimerDone = false;
    setupTimer(0);
}

pair<string, int> gameMessage()
{
    string gameMsg = "";
    int gameMsgSize = 0;

    if (playerReady && !quizStarted && !wrTimerDone)
    {
        gameMsg += "The game will start in 60 seconds.\n";
        gameMsgSize += gameMsg.length();
    }
    if (playerReady && !quizStarted && wrTimerDone)
    {
        gameMsg += "You're ready to play the game. Have fun!\n";
        gameMsgSize += gameMsg.length();
        // TODO: change so wait for all timer expire or set number of players joined
        // if (quiz.getTeam().size() == 4) {quizStarted = true;}
        quizStarted = true;
        cout << "Quiz started: " << quizStarted << endl;
        serverMessage = false;
    }

    if ((!playerReady && seeLeaderboard) || (!wrTimerDone && seeLeaderboard))
    {
        cout << "getting leaderboard" << endl;
        gameMsg += "\nLeaderboard\n";

        int index = 10;

        if (leaderboard.size() < 10)
        {
            index = leaderboard.size();
        }

        for (int i = 0; i < index; i++)
        {
            gameMsg += to_string(i+1);
            gameMsg +=  ") ";
            gameMsg += leaderboard.at(i).first;
            gameMsg += " - ";
            gameMsg += to_string(leaderboard.at(i).second);
            gameMsg += "\n";
            }

        cout << "Send to client leaderboard: " << gameMsg <<endl;
        gameMsgSize += gameMsg.length();
        seeLeaderboard = false;
    }

    if ((!playerReady && seePlayers) || (!wrTimerDone && seePlayers))
    {
        cout << "getting players" << endl;
        gameMsg += "\nPlayers in lobby\n";

        for (int i = 0; i < players.size(); i++)
        {
            gameMsg += players.at(i).getUsername();
            gameMsg += "\n";
            }

        cout << "Send to client player names: " << gameMsg <<endl;
        gameMsgSize += gameMsg.length();
        seePlayers = false;
    }

    if (quizStarted && questionSent && requestQuestionRepeat)
    {
        requestQuestionRepeat = false;
        if (!quiz.isQuestionSetEmpty())
        {
            gameMsg += quiz.getQuestionFromClass();
            gameMsgSize += gameMsg.length();
        }
    }

    if (quizStarted && questionSent && answerRec)
    {
        // TODO: change so wait for all teams response or timer expire
        answerRec = false;
        quiz.calculateMode(); // calculate most chosen response
        // cout << "MODE: " << quiz.getMode() << endl;
        quiz.checkCorrectResponse(); // check if most chosen response was correct or not
        quiz.setPointsOrFail();

        gameMsg += quiz.printAnswerScreen() + "\n";
        gameMsgSize += gameMsg.length();
        quiz.clearResponsesAndLastQuestion(); // clear responses for next question
        questionSent = false;
    }

    if (quizStarted && !questionSent)
    {
        if (!quiz.isQuestionSetEmpty())
        {
            cout << "Questions remain\n"
                 << endl;

            gameMsg += quiz.getQuestionFromClass();
            cout << "Next question: \n"
                 << gameMsg << endl;
            gameMsgSize += gameMsg.length();
            questionSent = true;
        }
        else
        {
            gameOver = true;
            // endGameCleanup(sock, buffer, size, bytesSent);
        }
    }

    return make_pair(gameMsg, gameMsgSize);
}

void loadLeaderboard()
{
    string line;      // line from file
    string name;
    int score;

    ifstream fileIn(LEADERBOARDFILE, ios::in);

    // file not found error
    if (!fileIn)
    {
        cerr << "error: open file for input failed!" << endl;
        return;
    }

    while (fileIn >> name >> score)
    {
        cout << "Name: " << name << "\nScore: " << score << "\n" << endl;
        leaderboard.push_back(make_pair(name, score));
    }

    fileIn.close();
    leaderboardLoaded = true;
    sort(leaderboard.begin(), leaderboard.end(), sortPairSecond);

}

void updateLeaderboard(int score, string name)
{
    bool nameFound = false;
    // cout << "UPDATE LEADERBOARD\n" << endl;

    int index;

    for (int i = 0; i < leaderboard.size(); i++)
    {
        if (leaderboard.at(i).first == name)
        {
            index = i;
            cout << "index: " << to_string(index) << endl;
            nameFound = true;
        }
    }

    if (nameFound) {
        if (leaderboard.at(index).second < score)
        {
            leaderboard.erase(leaderboard.begin() + index);
        }
    }

    leaderboard.push_back(make_pair(name, score));
    sort(leaderboard.begin(), leaderboard.end(), sortPairSecond);

    // write updated leaderboard to file
    fstream outFile;

    outFile.open(LEADERBOARDFILE, ios::trunc | ios::out);

    string newLeaderBoard = "";

    for (int i = 0; i < leaderboard.size(); i++)
    {
        newLeaderBoard += leaderboard.at(i).first;
        newLeaderBoard += " ";
        newLeaderBoard += to_string(leaderboard.at(i).second);
        newLeaderBoard += "\n";
    }

    outFile << newLeaderBoard;
    outFile.close();



}

bool sortPairSecond(const pair<string,int> &a, const pair<string,int> &b)
{
    return (a.second > b.second);
}

void setupTimer(int countdownLength) {
    start = time(0);  // current time
    countdown = start + countdownLength;  // set countdown to 30 seconds
    previousSecond = countdown - start;  // used so it doesn't print multiple times in one second
    timeLeft = previousSecond;

    if (!wrTimerDone)
        cout << "The game will start in 60 seconds." << endl;
    else {
        action = (chattingTime == true) ? "chat." : "answer.";
        cout << "You have " << previousSecond << " seconds to " << action << endl;
    }
}
