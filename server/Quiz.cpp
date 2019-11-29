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
    vector<Player> kickVotes;
    Player kicked;


public:
    Quiz(/* args */)
    {
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
        cout << "Mode was: " << responses[maxRepeat] << "\n" << endl;
        setMode(responses[maxRepeat]);
        // return responses[maxRepeat];
    }

    pair<string, string> findCandidate() 
    { 
        Player arr[kickVotes.size()];
        copy(kickVotes.begin(), kickVotes.end(), arr);
        int maj_index = 0, count = 1; 
        for (int i = 1; i < kickVotes.size(); i++) 
        { 
            if (arr[maj_index].getUsername() == arr[i].getUsername() && arr[maj_index].getPassword() == arr[i].getPassword()) {
                count++; 
            }
            else {
                count--; 
            }
            if (count == 0) 
            { 
                maj_index = i; 
                count = 1; 
            } 
        } 
        pair<string, string> playerCand;
        playerCand = make_pair(arr[maj_index].getUsername(), arr[maj_index].getPassword());
        // return arr[maj_index];
        return playerCand; 
    } 
    
    /* Function to check if the candidate 
    occurs more than n/2 times */
    bool isMajority(Player cand) 
    { 
        Player arr[kickVotes.size()];
        copy(kickVotes.begin(), kickVotes.end(), arr);

        int count = 0; 

        for (int i = 0; i < kickVotes.size(); i++) {
            if (arr[i].getUsername() == cand.getUsername() && arr[i].getPassword() == cand.getPassword()) {
                count++; 
            }
        }
            
        if (count > (kickVotes.size())/2) {
            return true; 
        }
        else {
            return false; 
        }
    } 

    /* Function to print Majority Element */
    bool findMajority() 
    { 
        /* Find the candidate for Majority*/
        pair<string, string> cand = findCandidate(); 

        Player playCand = Player(cand.first, cand.second);
        
        /* Print the candidate if it is Majority*/
        if (isMajority(playCand)) {
            kicked = playCand;

            return true;
        }
        else {
            return false;
        }
    } 

    void addKick(Player player) {
        kickVotes.push_back(player);
        
    }

    vector<Player> getKickVotes() { return kickVotes; }
    Player getKicked() { return kicked; }


/*
    Player potentialMajorityElement() 
    {
        Player major = kickVotes[0];
        int count = 1;

        for(int i=1; i<kickVotes.size();i++)
        {
            if(count==0)
            {
                count++;
                major=kickVotes[i];

            }
            else if(major.getPassword()==kickVotes[i].getPassword() && major.getUsername()==kickVotes[i].getUsername())
            {
                count++;
            }
            else
            { 
                count--;
            }
        }
        kicked = major;
        return major;
    }
    */

    void checkCorrectResponse() // check if majority choice is right or wrong
    {
        char correctAns = questionSet.back().getAnswer();
        char mode = getMode();

        cout << "Correct answer for question: " << correctAns << endl;

        setWasAnswerCorrect(correctAns == mode);
    }

    string printAnswerScreen() // print result of question
    {
        if (getAnswerCorrect())
        {
            // cout << "Congratulations, you picked the right answer" << endl;
            return "Congratulations, you picked the right answer\n";
        }
        else
        {
            // cout << "You picked the wrong choice" << endl;
            return "You picked the wrong choice\n";
        }
    }

    void setPointsOrFail()
    {
        if (getAnswerCorrect())
        {
            incPoints();
            cout << "Points = " << points << "\n" << endl;
        }
        else
        {
            incFailCounter();
            cout << "Fails = " << failCounter << "\n" << endl;
            if (failCounter >= 3)
            {
                clearQuestionSet();
            }
        }
    }

    // setters
    void setFailCounter(int cnt) { failCounter = cnt; }
    void setPoints(int pnt) { points = pnt; }
    void setMode(char mChar) { mode = mChar; }
    void setWasAnswerCorrect(bool ans) { answerCorrect = ans; }

    void clearResponsesAndLastQuestion() 
    { 
        responses.clear(); 
        if (!questionSet.empty())
        {
            questionSet.pop_back();
        }   
    }

    void clearQuestionSet() { questionSet.clear(); }
    void clearTeam() 
    { 
        team.clear(); 
        setPoints(0);
        setFailCounter(0);
    }

    void incPoints() { points++; }
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

    // getters
    char getMode() { return mode; } // most chosen option
    int getPoints() { return points; }
    bool getAnswerCorrect() { return answerCorrect; }
    int getFailCounter() { return failCounter; }
    string getQuestionFromClass() { return questionSet.back().getQuestionsAndChoicesString(); }
    bool isQuestionSetEmpty() { return questionSet.empty(); }
    vector<Question> getQuestionSet() { return questionSet; }
    vector<Player> getTeam() { return team; }
    vector<char> getResponses() { return responses; }

    bool lastQuestion()
    {
        if (questionSet.size()==1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

};
