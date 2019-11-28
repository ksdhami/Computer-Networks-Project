#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Question.cpp"
#include "Player.cpp"

using namespace std;

// template <>
// struct less<Player>
// {

//     bool operator()(const Player &one, const Player &two) const { return one.password < two.password; }
// };

class Quiz
{
private:
    vector<Question> questionSet;                 // set of questions with their choices and correct answer
    vector<char> responses;                       // responses from players for a question; clear after each question
    char mode;                                    // most voted choice of question
    bool answerCorrect;                           // did players pick correct answer or not
    int failCounter;                              // total 3 wrong answers; stop quiz
    int points;                                   // points for each game
    vector<Player> team;                          // set of players currently playing game
    vector<Player> kickVotes;
    Player kicked;
    // map<Player, Player> kickVotesMap;             
    // map<Player, char> responsesMap;

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
        cout << "Mode was: " << responses[maxRepeat] << "\n"
             << endl;
        setMode(responses[maxRepeat]);
        // return responses[maxRepeat];
    }

    void calculateKicked() 
    {
        int *repCnt = new int[kickVotes.size()];
        for (size_t i = 0; i < kickVotes.size(); ++i)
        {
            repCnt[i] = 0;
            int j = 0;

            while ((j < i) && (kickVotes[i].getUsername() != kickVotes[j].getUsername()))
            {
                if (kickVotes[i].getUsername() != kickVotes[j].getUsername())
                {
                    ++j;
                }
            }
            ++(repCnt[j]);
        }

        int maxRepeat = 0;
        for (int i = 1; i < kickVotes.size(); ++i)
        {
            if (repCnt[i] > repCnt[maxRepeat])
            {
                maxRepeat = i;
            }
        }

        delete[] repCnt;
        cout << "Mode was: " << kickVotes[maxRepeat].getUsername() << "\n"
             << endl;
        setKicked(kickVotes[maxRepeat]);
    }

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
            cout << "Points = " << points << "\n"
                 << endl;
        }
        else
        {
            incFailCounter();
            cout << "Fails = " << failCounter << "\n"
                 << endl;
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
    void setKicked(Player player) { kicked = player; }

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

    void addKick(Player player) {
        kickVotes.push_back(player);
        
    }

    // void addToKick(string name, string pass) {

    //     auto attr_iter = find_if(players.begin(), players.end(), FindAttribute(to_string(clientAddr.sin_port)));
    //     if (attr_iter != players.end())
    //     {
    //         cout << "player found with password: " << to_string(clientAddr.sin_port) << endl;
    //         auto index = distance(players.begin(), attr_iter);
    //         cout << "index of player is: " << index << endl;
    //         quiz.addPlayer(players.at(index));

    //         // only set all player ready bool if size of team is right
    //         if (quiz.getTeam().size() == NUMPLAYERS)
    //         {
    //             playerReady = true;
    //         }
    //     }
    // }

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
    vector<Player> getKickVotes() { return kickVotes; }
    Player getKicked() { return kicked; }
    // map<Player, char> getResponsesMap() { return responsesMap; }

    bool lastQuestion()
    {
        if (questionSet.size() == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // MAP FUNCTIONS NOT IN USE
    /*
    void setInitResponsesMap()
    {
        responsesMap[team.at(0)] = ' ';
        responsesMap[team.at(1)] = ' ';
        responsesMap[team.at(2)] = ' ';
    }

    void addResponsesMap(char res, string name, string pass)
    {
        cout << "adding responses to map" << endl;
        for (auto const &x : responsesMap)
        {
            if (x.first.username == name && x.first.password == pass)
            {
                cout << "ip: " << name << " map: " << x.first.username << endl;
                responsesMap[x.first] = res;
            }
        }
    }

    bool checkResponseMapFull()
    {
        for (auto const &x : responsesMap)
        {
            if (x.second == ' ')
            {
                return false;
            }
        }

        return true;
    }
    */
};
