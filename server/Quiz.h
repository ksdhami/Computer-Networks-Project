#include <string>
#include <vector>
#include "Question.h"

#ifndef QUIZ_H
#define QUIZ_H

using namespace std;

class Quiz
{
private:
    vector<Question *> questionSet; // set of questions with their choices and correct answer
    vector<char> responses;         // responses from players for a question; clear after each question
    char mode;                      // most voted choice of question
    bool answerCorrect;             // did players pick correct answer or not
    int failCounter;                // total 3 wrong answers; stop quiz
    int points;                     // points for each game

public:
    Quiz(/* args */);
    ~Quiz();
    char calculateMode();        // calculate the most chosen choice from players
    void checkCorrectResponse(); // check if majority choice is right or wrong
    void printAnswerScreen();    // print result of question

    // setters
    void setMode(char mChar) { mode = mChar; }
    void setAnswerCorrect(bool ans) { answerCorrect = ans; }
    void addResponse(char res) { responses.push_back(res); }
    void clearResponses() { responses.clear(); }
    void incFailCounter() { failCounter++; }

    // getters
    char getMode() { return mode; }
    bool getAnswerCorrect() { return answerCorrect; }
    int getFailCounter() { return failCounter; }
};

#endif