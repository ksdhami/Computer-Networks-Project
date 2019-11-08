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

#define STDIN 0

const int BUFFERSIZE = 512;   // Size the message buffers
bool get = false;
bool list = false;
char type;
bool noSend = false;
bool noRecv = false;

fd_set tempRecvSockSet;            // Temp. receive socket set for select()
struct timeval timeout = {0, 10};  // The timeout value for select()
    struct timeval selectTime;
fd_set recvSockSet;   // The set of descriptors for incoming connections
int maxDesc = 1;      // The max descriptor
	
void processSockets();

int main(int argc, char *argv[])
{
    int sock;                        // A socket descriptor
    struct sockaddr_in serverAddr;   // Address of the server
    char inBuffer[BUFFERSIZE];       // Buffer for the message from the server
    int bytesRecv;                   // Number of bytes received
    
    char outBuffer[BUFFERSIZE];      // Buffer for message to the server
	char tempBuffer[BUFFERSIZE];
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
        
	
	
	
    
    int len;
    cout << "Please enter a message to be sent to the server: ";
	//while (strncmp(outBuffer, "/logout", 6) != 0)
    while (1)
    {   
		FD_ZERO(&recvSockSet);

    // Add the listening socket to the set
    FD_SET(sock, &recvSockSet);
	FD_SET(STDIN, &recvSockSet);
    maxDesc = max(maxDesc, sock);
	maxDesc = max(maxDesc, STDIN);
    
    socklen_t size = sizeof(serverAddr);
		// copy the receive descriptors to the working set
        memcpy(&tempRecvSockSet, &recvSockSet, sizeof(recvSockSet));
        
        // Select timeout has to be reset every time before select() is
        // called, since select() may update the timeout parameter to
        // indicate how much time was left.
        selectTime = timeout;
        int ready = select(maxDesc + 1, &tempRecvSockSet, NULL, NULL, &selectTime);
        if (ready < 0)
        {
            cout << "select() failed: " << errno << endl;
            break;
        }
		
		
		
		if (FD_ISSET(sock, &tempRecvSockSet))
        {
			//cout << "its set" << endl;
            // set the size of the client address structure
            

            // Establish a connection
            string msgReceived;
			while(1){
				memset(&inBuffer, 0, BUFFERSIZE);
				bytesRecv = recv(sock, (char *) &inBuffer, BUFFERSIZE, 0);
				cout << "bytesrecv: " << bytesRecv << endl;
				if (bytesRecv <= 0) {
					cout << "got nothing" << endl;
					break;
				}
				string temp(inBuffer);
				msgReceived += temp;
				if(bytesRecv < BUFFERSIZE){
					break;
				}
				
				
			}
			if(inBuffer[0] == '\0'){
				//cout << "aasdf" << endl;
				memset(&inBuffer, 0, BUFFERSIZE);
				continue;
			
            }
			cout << msgReceived;
            // Add the new connection to the receive socket set
            //FD_SET(clie, &recvSockSet);
            //maxDesc = max(maxDesc, clientSock);
			
        } 
		
    //fgets(outBuffer, BUFFERSIZE, stdin);
		if (FD_ISSET(STDIN, &tempRecvSockSet))
        {
			cout << "its set stdin" << endl;
            // set the size of the client address structure
            

            // Establish a connection
            memset(&outBuffer, 0, BUFFERSIZE);
			read(STDIN, outBuffer, BUFFERSIZE);
			/*string msgReceived;
			if(msgReceived.length() <= 0){
				cout << "no msg, moving on" << endl;
				memset(&outBuffer, 0, BUFFERSIZE);
            memset(&inBuffer, 0, BUFFERSIZE);
			memset(&tempBuffer, 0, BUFFERSIZE);
				continue;
			}*/
			//strcpy(outBuffer, msgReceived.c_str());
			//cout << "Server: " << msgReceived;
            
            // Add the new connection to the receive socket set
            //FD_SET(clie, &recvSockSet);
            //maxDesc = max(maxDesc, clientSock);
			if(outBuffer[0] == '\0' || outBuffer[0] == '\n'){
			cout << "aasdfOut" << endl;
			//memset(&outBuffer, 0, BUFFERSIZE);
			memset(&outBuffer, 0, BUFFERSIZE);
            memset(&inBuffer, 0, BUFFERSIZE);
			memset(&tempBuffer, 0, BUFFERSIZE);
			continue;
			}
           
        } 
		
		else{processSockets();}
		
		if(outBuffer[0] == '\0'){
			//cout << "aasdfOut" << endl;
			//memset(&outBuffer, 0, BUFFERSIZE);
			memset(&outBuffer, 0, BUFFERSIZE);
            memset(&inBuffer, 0, BUFFERSIZE);
			memset(&tempBuffer, 0, BUFFERSIZE);
			continue;
		}
		
		
		
		
		
		len = 0;
		
		if(strncmp(outBuffer, "/", 1) == 0) {
			//cout << "found possible command" << outBuffer << endl;
			
			for (int i = 0; i < BUFFERSIZE; i++) {
				if ((outBuffer[i] != '\0') && (outBuffer[i] != '\n') && (outBuffer[i] != ' ')) {
					len++;
				} else {
					break;
				}
			}
			//cout << "len: " << len << endl;
			if(strncmp(outBuffer, "/ready", len) == 0) {
				type = 'r';
			}else if(strncmp(outBuffer, "/leaderboard", len) == 0) {
				type = 'l';
			}else if(strncmp(outBuffer, "/players", len) == 0) {
				type = 'p';
			}else if(strncmp(outBuffer, "/help", len) == 0) {
				type = 'h';
				//cout << "/ready\n/leaderboard\n/players\n/help\n/extra\n/kick <playerName>\n/question\n/<answer>\n/leave\n/logout\n" << endl;
			}else if(strncmp(outBuffer, "/extra", len) == 0) {
				type = 'e';
			}else if(strncmp(outBuffer, "/kick <playerName>", len) == 0) {
				type = 'k';
			}else if(strncmp(outBuffer, "/question", len) == 0) {
				type = 'q';
			}else if(strncmp(outBuffer, "/answer", len) == 0) {
				type = 'a';
			}else if(strncmp(outBuffer, "/leave", len) == 0) {
				type = 'v';
			}else if(strncmp(outBuffer, "/logout", len) == 0) {
				type = 'o';
			}else{
				cout << "Unknown command. Type /help to list all commands." << endl;
				//continue;
			}
		}else{
			cout << "found a msg" << endl;
			type = 'm';
		}
		
		if (type == 'm') {
		
		}else if (type == 'r'){
			
		}else if (type == 'l'){
			
		}else if (type == 'p'){
			
		}else if (type == 'h'){
			cout << "/ready\n/leaderboard\n/players\n/help\n/extra\n/kick <playerName>\n/question\n/<answer>\n/leave\n/logout" << endl;
			noSend = true;
			//noRecv = true;
		}else if (type == 'e'){
			
		}else if (type == 'k'){
			
		}else if (type == 'q'){
			
		}else if (type == 'a'){
			
		}else if (type == 'v'){
			
		}else if (type == 'o'){
			
		}else{
		cout << "missing event handler" << endl;
	}
		//cout << "type: " << type << endl;
		// prepend type code
		if (!noSend){
			cout << "time to send" << endl;
			tempBuffer[0] = type;
			for(int i = 1; i < BUFFERSIZE + 1; i++){
				tempBuffer[i] = outBuffer[i-1];
			}
			
			msgLength = strlen(tempBuffer);
			
			// Send the message to the server
			bytesSent = send(sock, (char *) &tempBuffer, msgLength, 0);
			if (bytesSent < 0 || bytesSent != msgLength)
			{
				cout << "error in sending" << endl;
				exit(1); 
			}else{
				cout << "bytes sent: " << bytesSent << endl;
			}
			
		}
		noSend = false;
		
		
        
        
       
        if (type == 'o') {
			break;
		}
		/*
        if(!noRecv){
			string msgReceived = "";
				while(1){
					memset(&inBuffer, 0, BUFFERSIZE);
					bytesRecv = recv(sock, (char *) &inBuffer, BUFFERSIZE, 0);
					string temp(inBuffer);
					msgReceived += temp;
					if(bytesRecv < BUFFERSIZE){
						break;
					}
				}
				
				cout << msgReceived;
				
		}
		//cout << "type: " << type << endl;
		
		noRecv = false;*/
			// Clear the buffers
            memset(&outBuffer, 0, BUFFERSIZE);
            memset(&inBuffer, 0, BUFFERSIZE);
			memset(&tempBuffer, 0, BUFFERSIZE);

    
		cout << "Please enter a message to be sent to the server: ";
	}
	if (type == 'o') {
		string msgReceived = "";
		while(1){
			memset(&inBuffer, 0, BUFFERSIZE);
			bytesRecv = recv(sock, (char *) &inBuffer, BUFFERSIZE, 0);
			string temp(inBuffer);
			msgReceived += temp;
			if(bytesRecv < BUFFERSIZE){
				break;
			}
		}
	}
	
    // Close the socket
    close(sock);
    exit(0);
	
}

void processSockets(){
	
	
	
}

