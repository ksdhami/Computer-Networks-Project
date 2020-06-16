# Computer Networks Project

A quiz game built using TCP/IP. Clients can connect to the server and enter into the waiting room where they can communicate with one another and ready-up for the game. Once in the game, clients work together to solve trivia questions. A voting system is employed to determine the given response for each question. A leaderboard is kept, showing the highest scores achieved by all those who have played.


## Deployement 

To compile use the provided make files in the server and client folders

Note: while testing in RAC cloud environment, c++11 was used and is indicated in the makefile.
This may need to change depending on the system that will be used to test the progarm

To run the server:
```
./server <program port number>
```

To run the client:
```
./client <server address> <program port number>
```

## Contributers
* Xia Liu
* Shahmir Khan
* Katie Tieu
* Karndeep Dhami
