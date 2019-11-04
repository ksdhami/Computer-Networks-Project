#include <string>
#include <iostream>
#include "Question.h"

using namespace std;

Question::Question(string qesIn, string choiceAIn, string choiceBIn, string choiceCIn, string choiceDIn, char ansIn)
{
    // cout << "From Constructor \nA = " << choiceAIn << "\nB = " << choiceBIn << "\nC = " << choiceCIn << "\nD = " << choiceDIn << endl; 
    setQuestion(qesIn);
    setChoiceA(choiceAIn);
    setChoiceB(choiceBIn);
    setChoiceC(choiceCIn);
    setChoiceD(choiceDIn);
    setAnswer(ansIn);
}

Question::~Question()
{
    
}

// string that will show question and choices 
string Question::getQuestionsAndChoicesString()
{
    string questionsAndChoices = getQuestion() + "\n" + getChoiceA() + "\n" + getChoiceB() + "\n" + getChoiceC() + "\n" + getChoiceD();

    return questionsAndChoices;
}
