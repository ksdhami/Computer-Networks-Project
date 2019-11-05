#include <string>
#include <vector>
#include "Question.h"
#include "Player.h"

#ifndef QUIZ_H
#define QUIZ_H

using namespace std;

class Quiz
{
private:
    vector<Question> questionSet; // set of questions with their choices and correct answer
    vector<char> responses;       // responses from players for a question; clear after each question
    char mode;                    // most voted choice of question
    bool answerCorrect;           // did players pick correct answer or not
    int failCounter;              // total 3 wrong answers; stop quiz
    int points;                   // points for each game
    vector<Player> team;          // set of players currently playing game

public:
    Quiz(/* args */);
    ~Quiz();
    void calculateMode();        // calculate the most chosen choice from players
    void checkCorrectResponse(); // check if majority choice is right or wrong
    string printAnswerScreen();    // print result of question
    void setPointsOrFail();

    // setters
    void setFailCounter(int cnt) { failCounter = cnt; }
    void setPoints(int pnt) { points = pnt; }
    void setMode(char mChar) { mode = mChar; }
    void setAnswerCorrect(bool ans) { answerCorrect = ans; }
    void clearResponses() { responses.clear(); }
    void incFailCounter() { failCounter++; }
    void addQuestion(Question ques); // add question to set for quiz
    void addPlayer(Player person);   // add person to team for game
    void addResponse(char res);      // add each players choice to set
    void removeQuestion();

    // getters
    char getMode() { return mode; } // most chosen option
    int getPoints() { return points; }
    bool getAnswerCorrect() { return answerCorrect; }
    int getFailCounter() { return failCounter; }
    Question getQuestion() { return questionSet.back(); }
    bool isQuestionSetEmpty() { return questionSet.empty(); }
    vector<Question> getQuestionSet() { return questionSet; }
};

#endif