#include <iostream>
#include "Quiz.h"

using namespace std;

Quiz::Quiz(/* args */)
{
    // TODO: finish constructor
    failCounter = 0;
}

Quiz::~Quiz()
{
    // TODO: deconstructor
}

char Quiz::calculateMode()
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
    // for even number of users
    // TODO: change to even number of responses insead
    if (responses.size() % 2 == 0)
    {
        repCnt[0] = repCnt[0] + 1;
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
    return responses[maxRepeat];
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

// add question to set
void Quiz::addQuestion(Question *ques)
{
    // cout << "Question being added: \n" << ques->getQuestionsAndChoicesString() << endl;
    // cout << "Answer to question is: " << ques->getAnswer() << endl;
    questionSet.push_back(ques);
}