#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>
#include <ctime>
#include <chrono>
using namespace std;

bool countdown(long int countdownTime);

int main(int argc, char *argv[]){
	
	countdown(5);
}

bool countdown(long int countdownTime){
	time_t start = time
	
	//std::clock_t start, current, done;

	auto current = start;
	auto done = start + countdownTime;
    /* Your algorithm here */

    while(current < done) {
		current = std::chrono::system_clock::now();
	}

    cout << "done" << endl;
	
	
}