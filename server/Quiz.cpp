#include <iostream>
#include "Quiz.h"

using namespace std;

Quiz::Quiz(/* args */)
{
    // TODO: finish constructor
    setFailCounter(0);
    setPoints(0);
}

Quiz::~Quiz()
{

}

void Quiz::calculateMode()
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

// check if responses by players is right or wrong
void Quiz::checkCorrectResponse()
{
    Question *ans = questionSet.at(0);
    char correctAns = ans->getAnswer();

    cout << "Correct answer for question: " << correctAns << endl;

    setAnswerCorrect(correctAns == getMode());
}

// print result of response
void Quiz::printAnswerScreen()
{
    if (getAnswerCorrect())
    {
        cout << "Congratulations, you picked the right answer" << endl;
    }
    else
    {
        cout << "You picked the wrong choice" << endl;
    }
}

void Quiz::addResponse(char res)
{
    // cout << "Chocie being added to set of responses: " << res << endl;
    responses.push_back(res);
}

// add question to set
void Quiz::addQuestion(Question *ques)
{
    // cout << "Question being added: \n" << ques->getQuestionsAndChoicesString() << endl;
    // cout << "Answer to question is: " << ques->getAnswer() << endl;
    questionSet.push_back(ques);
}

// add player to team 
void Quiz::addPlayer(Player *person)
{
    // cout << "User being added to team: " << person->getUsername() << " with password: " << person->getPassword() << endl;
    team.push_back(person);
}