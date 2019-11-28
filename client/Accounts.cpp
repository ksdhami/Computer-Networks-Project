//Libraries included for this file
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
using namespace std;


class Accounts{
public:
// Method Login to the check if users has correct credentials to log in
  void Login()
  {

    int counter = 0;
    int count2 = 0;

    user_found = false;
    pass_found = false;

    // Get user input for username
    cout << "Enter Username: ";
    cin >> user;

    //Check to see if the user is in fi;e users.dat and store the position in counter
    std::string line;
    ifstream user_file ("users.dat");
    if (user_file.is_open()){
      while (getline(user_file,line))
      {
        if (!user.compare(line)) {
          userName = user;
          user_found = true;
          break;
        }
        counter++;
      }

      user_file.close();
    }

    //If user was found then ask for password
    if (user_found == true) {
      cout << "Enter Password: ";
      cin >> pass;

      //CHeck to see if that password is in the correct position in file users2.dat
      std::string line2;
      ifstream user_file ("users2.dat");
      if (user_file.is_open()) {
        while (std::getline(user_file,line2))
        {
            if (!pass.compare(line2) && (counter == count2)) {
              cout << "Correct" << endl;
              passWord = pass;
              pass_found = true;
              break;
            }
            count2++;
        }
      }
      user_file.close();

      // Check to see if password was found or not, if not then repeat
      if (pass_found == false) {
        cout << "Username or Password is Incorrect." << endl;
        Login();
      }

    }

    // Check to see if the username was found or not, if not then repeat
    else if (user_found == false){
      cout << "Enter Password: ";
      cin >> pass;
      cout << "Username or Password is Incorrect." <<endl;
      Login();
    }


  };
  Accounts(){};
  // Method to check if user logged in with correct credentials
  bool isUserValid()
  {
    if (user_found == true && pass_found == true)
    { // user and password found, return true
      return true;
    }
    else { // Either username or password not found, return false
      return false;
    }
  };
  //getter method to return the username
  string getUser()
  {
      return userName;
  };
  //getter method to return the password
  string getPassword()
  {
    return passWord;
  };
  // Method to register a new user into the system
  void Register()
  {
    cout << "Enter your new Username: ";
    cin >> user;
    // Check to see if username exists already or not in file users.dat
    user_found = false;
    std::string line;
    ifstream user_file ("users.dat");
    if (user_file.is_open())
    {
        while (getline(user_file,line))
        {
          if (!user.compare(line))
          {
            user_found = true;
            break;
          }
        }
    }
    user_file.close();

    //If username was not found in users.dat then add the username to users.dat
    if (user_found == false) {
      userName = user;
      //user_found = true;
      //pass_found = true;
      ofstream outfile;
      outfile.open("users.dat", std::ios_base::app);
      outfile << userName+"\n";
      outfile.close();

      //Add a corresponding password to users2.dat
      cout << "Enter your new Password: ";
      cin >> passWord;
      outfile.open("users2.dat", std::ios_base::app);
      outfile << passWord+"\n";
      outfile.close();
    }
    else
    {
      // Repeat the process for login for unsuccesfull registration attempt
      cout << "This username is taken, select a different one." <<endl;
      Register();
    }

  };

private:
  string userName;  // Username of the user
  string passWord;  // Password of the user
  string user;  // Temporary username of user
  string pass;  // Temporary password of user
  bool user_found;  // Check to see if username was found
  bool pass_found;  // Check to see if password was found

};
