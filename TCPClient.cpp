/*
 * A simple TCP client that sends messages to a server and display the message
   from the server. 
 * For use in CPSC 441 lectures
 * Instructor: Prof. Mea Wang
 */

#include <iostream>
#include <sys/socket.h> // for socket(), connect(), send(), and recv()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <stdio.h>
#include <fstream> 

using namespace std;

const int BUFFERSIZE = 32;   // Size the message buffers
bool get = false;
bool list = false;

int main(int argc, char *argv[])
{
    int sock;                        // A socket descriptor
    struct sockaddr_in serverAddr;   // Address of the server
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

    // Create a TCP socket
    // * AF_INET: using address family "Internet Protocol address"
    // * SOCK_STREAM: Provides sequenced, reliable, bidirectional, connection-mode byte streams.
    // * IPPROTO_TCP: TCP protocol
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
    
    // Connect to the server
    // * sock: the socket for this connection
    // * serverAddr: the server address
    // * sizeof(*): the size of the server address
    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "connect() failed" << endl;
        exit(1);
    }
        

    cout << "Please enter a message to be sent to the server ('logout' to terminate): ";
    fgets(outBuffer, BUFFERSIZE, stdin);
    while (strncmp(outBuffer, "logout", 6) != 0)
    {   
		if(strncmp(outBuffer, "get"	, 3) == 0){
			//cout << "gets true" << endl;
			get = true;
		}
        msgLength = strlen(outBuffer);
        
        // Send the message to the server
        bytesSent = send(sock, (char *) &outBuffer, msgLength, 0);
        if (bytesSent < 0 || bytesSent != msgLength)
        {
            cout << "error in sending" << endl;
            exit(1); 
        }
		
		if(list){
		
			string msgReceived = "";
			while(1){
				memset(&inBuffer, 0, BUFFERSIZE);
				bytesRecv = recv(sock, (char *) &inBuffer, 32, 0);
				string temp(inBuffer);
				msgReceived += temp;
				if(bytesRecv < 32){
					break;
				}
			}
			
			cout << "Server: " << msgReceived;
		}
		
		if(get){
            memset(&inBuffer, 0, BUFFERSIZE);
			bytesRecv = recv(sock, (char *) &inBuffer, 32, 0);
            string name(inBuffer);
            //cout << "name = " << name << endl;
            //cout << "name = " << name << endl;
            
            
            
			ofstream outfile(name, ofstream::binary);
			int bytesRecvTotal = 0;
			while(1){
                
				memset(&inBuffer, 0, BUFFERSIZE);
				bytesRecv = recv(sock, (char *) &inBuffer, 32, 0);
                bytesRecvTotal += bytesRecv;
				//string temp(inBuffer);
				//msgReceived += temp;
				outfile.write(inBuffer, 32);
				if(bytesRecv < 32){
					break;
				}
			}
			cout << "File saved in " << name << " (" << bytesRecvTotal << " bytes)" << endl;
			memset(&outBuffer, 0, BUFFERSIZE);
			/* outBuffer[0] = 'd';
			bytesSent = send(sock, (char*) &outBuffer, 1, 0);
			if (bytesSent < 0 || bytesSent != 1)
			{
				cout << "error in sending" << endl;
				exit(1); 
			} */
			
			/*int sizeOfFile = msgReceived.length();
			
			char* fileBuffer = new char[sizeOfFile];
			strcpy(fileBuffer, msgReceived.c_str());
			
			outfile.write(fileBuffer, sizeOfFile);
			
			delete[] fileBuffer;*/
			
			outfile.close();
			
			//get = false;
		}
        
        
        if (!get) {
            string msgReceived = "";
			while(1){
				memset(&inBuffer, 0, BUFFERSIZE);
				bytesRecv = recv(sock, (char *) &inBuffer, 32, 0);
				string temp(inBuffer);
				msgReceived += temp;
				if(bytesRecv < 32){
					break;
				}
			}
			
			cout << "Server: " << msgReceived;
        
		

            // Clear the buffers
            memset(&outBuffer, 0, BUFFERSIZE);
            memset(&inBuffer, 0, BUFFERSIZE);
            cout << "Please enter a message to be sent to the server ('logout' to terminate): ";
            fgets(outBuffer, BUFFERSIZE, stdin);
        } else {
            get = false;
            cout << "Please enter a message to be sent to the server ('logout' to terminate): ";
            fgets(outBuffer, BUFFERSIZE, stdin);
            
        }
    }

    // Close the socket
    close(sock);
    exit(0);
}

