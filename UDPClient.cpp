/*
 * A simple UDP client that sends messages to a server and display the message
   from the server.  This program assumes that each message is small enough to be 
   sent and received in one send and receive operation.
 * For use in CPSC 441 lectures
 * Instructor: Prof. Mea Wang
 */

#include <iostream>
#include <sys/socket.h> // for socket(), sendto(), and recvfrom()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <stdio.h>

using namespace std;

const int BUFFERSIZE = 32;   // Size the message buffers

int main(int argc, char *argv[])
{
    int sock;                        // A socket descriptor
    struct sockaddr_in serverAddr;   // Address of the server
    struct sockaddr_in fromAddr;     // Address of the sender of a message (expected to be same as the server)
    socklen_t size;               // Size of the client address

    char inBuffer[BUFFERSIZE];       // Buffer for the message from the server
    int bytesRecv;                   // Number of bytes received
    
    char outBuffer[BUFFERSIZE];      // Buffer for message to the server
    int msgLength;                   // Length of the outgoing message
    int bytesSent;                   // Number of bytes sent

    // Check for input errors
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <Server IP> <Server Port>" << endl;
        exit(1);
    }

    // Create a UDP socket
    // * AF_INET: using address family "Internet Protocol address"
    // * SOCK_DGRAM: supports datagrams (connectionless, unreliable messages of a fixed (typically small) maximum length)
    // * IPPROTO_UDP: UDP protocol
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
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
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        cout << "setsockopt() failed" << endl;
        exit(1);
    }
    
    // Initialize the server information
    // Note that we can't choose a port less than 1023 if we are not privileged users (root)
    memset(&serverAddr, 0, sizeof(serverAddr));         // Zero out the structure
    serverAddr.sin_family = AF_INET;                    // Use Internet address family
    serverAddr.sin_port = htons(atoi(argv[2]));         // Server port number
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);    // Server IP address
    
    cout << "Please enter a message to be sent to the server ('logout' to terminate): ";
    fgets(outBuffer, BUFFERSIZE, stdin);
    while (strncmp(outBuffer, "logout", 6) != 0)
    {   
        msgLength = strlen(outBuffer);
        
        // Send the message to the server
        bytesSent = sendto(sock, (char *) &outBuffer, msgLength, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
        //cout << "sent: " << bytesSent << endl;
        
        // Check for errors
        if (bytesSent != msgLength)
        {
            cout << "error in sending" << endl;
            exit(1); 
        }

        // set the size of the client address structure
        size = sizeof(fromAddr);
    
        // Receive the response from the server
        bytesRecv = recvfrom(sock, (char *) &inBuffer, BUFFERSIZE, 0, (struct sockaddr *) &fromAddr, &size);
        
        // Check for errors
        if (bytesRecv != msgLength)
        {
            cout << "recvfrom() failed, or the connection is closed. " << endl;
            exit(1); 
        }
        if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            cout << "receive from unknow source " << inet_ntoa(fromAddr.sin_addr) << endl;
            exit(1); 
        }
        inBuffer[bytesRecv] = '\0';
        cout << "Server: " << inBuffer;

        // Clear the buffers
        memset(&outBuffer, 0, BUFFERSIZE);
        memset(&inBuffer, 0, BUFFERSIZE);
        cout << "Please enter a message to be sent to the server ('logout' to terminate): ";
        fgets(outBuffer, BUFFERSIZE, stdin);
    }

    // Close the socket
    close(sock);
    exit(0);
}

