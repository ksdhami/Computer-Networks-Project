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

struct sockaddr_in serverAddrUDP; // address of the UDP server

bool unknownCommandFlag = false;
string unknownCommand;

void initServerTCP(int &, int port);
void processSockets(fd_set);
void sendDataTCP(int, char[], int);
void receiveDataTCP(int, char[], int &);

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

        if (~(TCPInc))
        {
        }
        else
        {
            processSockets(tempRecvSockSet);
        }

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
    else if (strncmp(inBuffer, "list", 4) == 0)
    {
        list = true;
    }
    else if (strncmp(inBuffer, "get", 3) == 0)
    {
        getFile = true;
        string temp(inBuffer);
        fileName = temp.substr(4, strlen(temp.c_str()) - 5);
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

    if (list)
    {
        // Execute the "ls" command and save the output to /tmp/temp.txt.
        // /tmp is a special directory storing temporate files in Linux.
        system("ls > /tmp/temp.txt");

        // Open the file
        ifstream infile;
        infile.open("/tmp/temp.txt");

        // Store the file content into a string
        string line;
        string data = "";
        while (getline(infile, line))
        {
            data += line + "\n";
        }

        // Close the file
        infile.close();

        cout << data;
        size = strlen(data.c_str());
        //cout << "size is " << size << endl;
        bytesSent = send(sock, (char *)data.c_str(), size, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
        //cout << "SENT: " << bytesSent << endl;
        list = false;
    }
    // Sent the data
    //bytesSent += send(sock, (char *) buffer + bytesSent, size - bytesSent, 0);
    if (getFile)
    {
        size_t found = fileName.find(".");

        std::string temp = "-" + std::to_string(clientAddr.sin_port);

        string newName = fileName;
        newName.insert(found, temp);

        bytesSent = send(sock, (char *)newName.c_str(), newName.length(), 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
        //cout << "SENT: " << bytesSent << endl;
        list = false;

        char *cstr = new char[fileName.length() + 1];
        strcpy(cstr, fileName.c_str());
        //cout << "FILENAME = '" << cstr << "'" << endl;
        std::ifstream infile(cstr, ifstream::binary);

        // get size of file
        infile.seekg(0, infile.end);
        //cout << "about to size=\n";
        ifstream::pos_type size = infile.tellg();
        //cout << "about to seekg\n";
        infile.seekg(0);
        //cout << "done\n";

        // allocate memory for file content
        char *buffer = new char[size];
        //cout << "im about to read a " << size << " file" << endl;
        // read content of infile
        infile.read(buffer, size);

        //cout << "read it\n";

        bytesSent = send(sock, (char *)buffer, size, 0);
        if (bytesSent < 0)
        {
            cout << "error in sending: sendDataTCP" << endl;
            return;
        }
        //cout << "SENT: " << bytesSent << endl;

        delete[] buffer;

        infile.close();
        getFile = false;
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
