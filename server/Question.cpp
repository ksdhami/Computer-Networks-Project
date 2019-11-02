#include <string>
#include "Question.h"

using namespace std;

Question::Question(string qes, string cA, string cB, string cC, string cD, char ans)
{
    setQuestion(qes);
    setChoiceA(cA);
    setChoiceB(cB);
    setChoiceC(cC);
    setChoiceD(cD);
    setAnswer(ans);
}

Question::~Question()
{
    // TODO: deconstructor
}

// string that will show question and choices 
string Question::getQuestionsAndChoicesString()
{
    string questionsAndChoices = getQuestion() + "\n" + getChoiceA() + "\n" + getChoiceB() + "\n" + getChoiceC() + "\n" + getChoiceD();

    return questionsAndChoices;
}
