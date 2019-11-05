#include <iostream>
#include <string>
#include <vector>
#include "Question.cpp"
#include "Player.cpp"
// #include "Quiz.h"

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
    Quiz(/* args */)
    {
        // TODO: finish constructor
        setFailCounter(0);
        setPoints(0);
    }

    ~Quiz()
    {
    }

    void calculateMode() // calculate the most chosen choice from players
    {
        // Allocate an int array of the same size to hold the repetition count
        int *repCnt = new int[responses.size()];
        for (size_t i = 0; i < responses.size(); ++i)
        {
            repCnt[i] = 0;
            int j = 0;

            while ((j < i) && (responses[i] != responses[j]))
            {
                if (responses[i] != responses[j])
                {
                    ++j;
                }
            }
            ++(repCnt[j]);
        }

        int maxRepeat = 0;
        for (int i = 1; i < responses.size(); ++i)
        {
            if (repCnt[i] > repCnt[maxRepeat])
            {
                maxRepeat = i;
            }
        }

        delete[] repCnt;
        setMode(responses[maxRepeat]);
        // return responses[maxRepeat];
    }

    void checkCorrectResponse() // check if majority choice is right or wrong
    {
        Question ques = questionSet.back();
        char correctAns = ques.getAnswer();

        cout << "Correct answer for question: " << correctAns << endl;

        setAnswerCorrect(correctAns == getMode());
    }

    string printAnswerScreen() // print result of question
    {
        if (getAnswerCorrect())
        {
            cout << "Congratulations, you picked the right answer" << endl;
            return "Congratulations, you picked the right answer";
        }
        else
        {
            cout << "You picked the wrong choice" << endl;
            return "You picked the wrong choice";
        }
    }

    void setPointsOrFail()
    {
        if (getAnswerCorrect())
        {
            setPoints(1);
        }
        else
        {
            incFailCounter();
        }
    }

    // setters
    void setFailCounter(int cnt) { failCounter = cnt; }
    void setPoints(int pnt) { points = pnt; }
    void setMode(char mChar) { mode = mChar; }
    void setAnswerCorrect(bool ans) { answerCorrect = ans; }
    void clearResponses() { responses.clear(); }
    void incFailCounter() { failCounter++; }

    void addQuestion(Question ques) // add question to set for quiz
    {
        cout << "Question being added: \n"
             << ques.getQuestionsAndChoicesString() << endl;
        // cout << "Answer to question is: " << ques->getAnswer() << endl;
        questionSet.push_back(ques);
    }

    void addPlayer(Player person) // add person to team for game
    {
        cout << "User being added to team: " << person.getUsername() << " with password: " << person.getPassword() << endl;
        team.push_back(person);
    }

    void addResponse(char res) // add each players choice to set
    {
        // cout << "Chocie being added to set of responses: " << res << endl;
        responses.push_back(res);
    }

    void removeQuestion()
    {
        questionSet.pop_back();
    }

    // getters
    char getMode() { return mode; } // most chosen option
    int getPoints() { return points; }
    bool getAnswerCorrect() { return answerCorrect; }
    int getFailCounter() { return failCounter; }
    Question getQuestion() { return questionSet.back(); }
    bool isQuestionSetEmpty() { return questionSet.empty(); }
    vector<Question> getQuestionSet() { return questionSet; }
};

// Quiz::Quiz(/* args */)
// {
//     // TODO: finish constructor
//     setFailCounter(0);
//     setPoints(0);
// }

// Quiz::~Quiz()
// {
// }

// void Quiz::calculateMode()
// {
//     // Allocate an int array of the same size to hold the repetition count
//     int *repCnt = new int[responses.size()];
//     for (size_t i = 0; i < responses.size(); ++i)
//     {
//         repCnt[i] = 0;
//         int j = 0;

//         while ((j < i) && (responses[i] != responses[j]))
//         {
//             if (responses[i] != responses[j])
//             {
//                 ++j;
//             }
//         }
//         ++(repCnt[j]);
//     }

//     int maxRepeat = 0;
//     for (int i = 1; i < responses.size(); ++i)
//     {
//         if (repCnt[i] > repCnt[maxRepeat])
//         {
//             maxRepeat = i;
//         }
//     }

//     delete[] repCnt;
//     setMode(responses[maxRepeat]);
//     // return responses[maxRepeat];
// }

// // check if responses by players is right or wrong
// void Quiz::checkCorrectResponse()
// {
//     Question ques = questionSet.back();
//     char correctAns = ques.getAnswer();

//     cout << "Correct answer for question: " << correctAns << endl;

//     setAnswerCorrect(correctAns == getMode());
// }

// // print result of response
// string Quiz::printAnswerScreen()
// {
//     if (getAnswerCorrect())
//     {
//         cout << "Congratulations, you picked the right answer" << endl;
//         return "Congratulations, you picked the right answer";
//     }
//     else
//     {
//         cout << "You picked the wrong choice" << endl;
//         return "You picked the wrong choice";
//     }
// }

// void Quiz::setPointsOrFail()
// {
//     if (getAnswerCorrect())
//     {
//         setPoints(1);
//     }
//     else
//     {
//         incFailCounter();
//     }
// }

// void Quiz::removeQuestion()
// {
//     questionSet.pop_back();
// }

// void Quiz::addResponse(char res)
// {
//     // cout << "Chocie being added to set of responses: " << res << endl;
//     responses.push_back(res);
// }

// // add question to set
// void Quiz::addQuestion(Question ques)
// {
//     cout << "Question being added: \n"
//          << ques.getQuestionsAndChoicesString() << endl;
//     // cout << "Answer to question is: " << ques->getAnswer() << endl;
//     questionSet.push_back(ques);
// }

// // add player to team
// void Quiz::addPlayer(Player person)
// {
//     cout << "User being added to team: " << person.getUsername() << " with password: " << person.getPassword() << endl;
//     team.push_back(person);
// }