#include <string>
#include <iostream>
// #include "Question.h"

using namespace std;

class Question
{
private:
    string question;
    string choiceA;
    string choiceB;
    string choiceC;
    string choiceD;
    char answer;

public:
    // Question() {};
    Question(string qesIn, string choiceAIn, string choiceBIn, string choiceCIn, string choiceDIn, char ansIn)
    {
        // cout << "From Constructor \nA = " << choiceAIn << "\nB = " << choiceBIn << "\nC = " << choiceCIn << "\nD = " << choiceDIn << endl;
        setQuestion(qesIn);
        setChoiceA(choiceAIn);
        setChoiceB(choiceBIn);
        setChoiceC(choiceCIn);
        setChoiceD(choiceDIn);
        setAnswer(ansIn);
    }
    ~Question()
    {
    }
    string getQuestionsAndChoicesString() // for repeating question and choices command
    {
        string questionsAndChoices = getQuestion() + "\n" + getChoiceA() + "\n" + getChoiceB() + "\n" + getChoiceC() + "\n" + getChoiceD();

        return questionsAndChoices;
    }

    // setters
    void setQuestion(string qesIn) { question = qesIn; }
    void setChoiceA(string choiceAIn) { choiceA = choiceAIn; }
    void setChoiceB(string choiceBIn) { choiceB = choiceBIn; }
    void setChoiceC(string choiceCIn) { choiceC = choiceCIn; }
    void setChoiceD(string choiceDIn) { choiceD = choiceDIn; }
    void setAnswer(char ansIn) { answer = ansIn; }

    // getters
    string getQuestion() { return question; }
    string getChoiceA() { return choiceA; }
    string getChoiceB() { return choiceB; }
    string getChoiceC() { return choiceC; }
    string getChoiceD() { return choiceD; }
    char getAnswer() { return answer; }
};

// Question::Question(string qesIn, string choiceAIn, string choiceBIn, string choiceCIn, string choiceDIn, char ansIn)
// {
//     // cout << "From Constructor \nA = " << choiceAIn << "\nB = " << choiceBIn << "\nC = " << choiceCIn << "\nD = " << choiceDIn << endl;
//     setQuestion(qesIn);
//     setChoiceA(choiceAIn);
//     setChoiceB(choiceBIn);
//     setChoiceC(choiceCIn);
//     setChoiceD(choiceDIn);
//     setAnswer(ansIn);
// }

// Question::~Question()
// {
// }

// // string that will show question and choices
// string Question::getQuestionsAndChoicesString()
// {
//     string questionsAndChoices = getQuestion() + "\n" + getChoiceA() + "\n" + getChoiceB() + "\n" + getChoiceC() + "\n" + getChoiceD();

//     return questionsAndChoices;
// }
