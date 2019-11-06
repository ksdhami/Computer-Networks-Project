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
#include "Quiz.cpp"
// #include "Question.cpp"

// #define QUESTIONFILE "questions.txt"
#define QUESTIONFILE "qq.txt"

using namespace std;

const int BUFFERSIZE = 32; // Size the message buffers
const int MAXPENDING = 10; // Maximum pending connections

fd_set recvSockSet; // The set of descriptors for incoming connections
int maxDesc = 0;    // The max descriptor
bool terminated = false;
bool list = false;
bool getFile = false;
string fileName = "";
char inBuffer[BUFFERSIZE]; // Buffer for the message from the server

int serverSockTCP;                // server socket descriptor
int clientSock;                   // client socket descriptor
struct sockaddr_in clientAddr;    // address of the client
struct timeval timeout = {0, 10}; // The timeout value for select()
struct timeval selectTime;
fd_set tempRecvSockSet; // Temp. receive socket set for select()

bool unknownCommandFlag = false;
string unknownCommand;

Quiz quiz;
bool quizStarted = false;
bool readQuestionsFromFile = false;
bool playerReady = false;
bool requestQuestionRepeat = false;
bool questionSent = false;
bool answerRec = false;
bool lastQuestion = false;
vector<Player> players;

void initServerTCP(int &, int port);
void processSockets(fd_set);
void sendDataTCP(int, char[], int);
void receiveDataTCP(int, char[], int &);

void readQuestionFile();
void quizGame();

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
            // Establish a connection
            if ((clientSock = accept(serverSockTCP, (struct sockaddr *)&clientAddr, &size)) < 0)
                break;
            cout << "Accepted a connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << endl;

            Player player(inet_ntoa(clientAddr.sin_addr), to_string(clientAddr.sin_port));
            players.push_back(player);
            cout << "Player being added to select server vector: " << players.back().getUsername() << " with password: " << players.back().getPassword() << endl;

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
        // else
        // {
        //     processSockets(tempRecvSockSet);
        // }

        TCPInc = false;
    }

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
    lastQuestion = false;
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
            receiveDataTCP(sock, buffer, size);
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

    string msg = string(inBuffer);
    cout << "TCP Client: " << msg;

    if (strncmp(inBuffer, "terminate", 9) == 0)
    {
        terminated = true;
    }
    else if (strncmp(inBuffer, "/ready", 6) == 0 && playerReady == false)
    {
        auto attr_iter = find_if(players.begin(), players.end(), FindAttribute(to_string(clientAddr.sin_port)));
        if (attr_iter != players.end())
        {
            cout << "player found with password: " << to_string(clientAddr.sin_port) << endl;
            auto index = distance(players.begin(), attr_iter);
            cout << "index of player is: " << index << endl;
            quiz.addPlayer(players.at(index));
            playerReady = true;
        }
    }
    else if (strncmp(inBuffer, "/question", 7) == 0)
    {
        requestQuestionRepeat = true;
    }
    else if (strncmp(inBuffer, "/answer", 7) == 0 && quizStarted && questionSent)
    {
        string temp(inBuffer);
        cout << "client replied with: " << temp << endl;
        cout << "client replied with: " << temp[8] << endl;
        quiz.addResponse(tolower(temp[8]));
        answerRec = true;
    }
    else
    {
        unknownCommandFlag = true;
        string temp(inBuffer);
        unknownCommand = temp;
        cout << "Unknown command: " << temp << endl;
    }
}

void sendDataTCP(int sock, char *buffer, int size)
{
    int bytesSent = 0; // Number of bytes sent

    if (playerReady && !quizStarted)
    {
        string msg = "You ready to play the game. Have fun\n";
        int size = msg.length();
        bytesSent = send(sock, (char *)msg.c_str(), size, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
        else
        {
            // TODO: for multiplayer only set quizstarted when everyone has readied up
            quizStarted = true;
        }
    }

    if (quizStarted && questionSent && requestQuestionRepeat)
    {
        requestQuestionRepeat = false;
        if (!quiz.isQuestionSetEmpty())
        {
            string msg = quiz.getQuestionFromClass();
            int size = msg.length();
            bytesSent = send(sock, (char *)msg.c_str(), size, 0);
            if (bytesSent < 0)
            {
                cout << "error in sending: sendDataTCP" << endl;
                return;
            }
        }
    }

    if (quizStarted && questionSent && answerRec)
    {
        answerRec = false;
        quiz.calculateMode(); // calculate most chosen response
        // cout << "MODE: " << quiz.getMode() << endl;
        quiz.checkCorrectResponse(); // check if most chosen response was correct or not
        quiz.setPointsOrFail();

        string msg = quiz.printAnswerScreen() + "\n";
        int size = msg.length();
        bytesSent = send(sock, (char *)msg.c_str(), size, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }

        quiz.clearResponsesAndLastQuestion(); // clear responses for next question
        questionSent = false;
    }

    if (quizStarted && !questionSent)
    {
        if (!quiz.isQuestionSetEmpty())
        {
            cout << "Questions remain\n" << endl;

            string msg = quiz.getQuestionFromClass();
            cout << "Next question: \n" << msg << endl;
            int size = msg.length();
            bytesSent = send(sock, (char *)msg.c_str(), size, 0);
            // cout << to_string(bytesSent) << endl;
            if (bytesSent < 0)
            {
                cout << "error in sending: sendDataTCP" << endl;
                return;
            }
            // cout << "msg success sent" << endl;
            questionSent = true;
        }
        else
        {
            string msg = "GAME OVER\n";
            msg += "Points: " + to_string(quiz.getPoints()) + " Fails: " + to_string(quiz.getFailCounter()) + "\n";
            int size = msg.length();
            bytesSent = send(sock, (char *)msg.c_str(), size, 0);
            if (bytesSent < 0)
            {
                cout << "error in sending: sendDataTCP" << endl;
                return;
            }
            playerReady = false;
        }
    }

    if (unknownCommandFlag)
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
        unknownCommandFlag = false;
    }
}

