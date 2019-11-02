#include <string>

#ifndef QUESTION_H
#define QUESTION_H

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
    Question(string qes, string cA, string cB, string cC, string cD, char ans);
    ~Question();
    string getQuestionsAndChoicesString();  // for repeating question and choices command

    // setters
    void setQuestion(string qes) { question = qes;}
    void setChoiceA(string cA) { choiceA = cA;}
    void setChoiceB(string cB) { choiceB = cB;}
    void setChoiceC(string cC) { choiceB = cC;}
    void setChoiceD(string cD) { choiceB = cD;}
    void setAnswer(char ans) { answer = ans;}
    
    // getters
    string getQuestion() {return question;}
    string getChoiceA() {return choiceA;}
    string getChoiceB() {return choiceB;}
    string getChoiceC() {return choiceC;}
    string getChoiceD() {return choiceD;}
    char getAnswer() {return answer;}
};

#endif