#include <iostream>
#include <algorithm>
#include <tuple>
#include <iterator>
#include <random>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>

using namespace std;

const int NUM_LEN = 4;
const string NUMBERS = "1234567890";

//shuffle NUMBERS and grab the first 4
string generate_digits() {
    string answer = NUMBERS;
    shuffle(answer.begin(), answer.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));
    answer = answer.substr(0, NUM_LEN);
    printf("Secret number generated\n");
    return answer;
}
//Checks for dupes digits/letters in a string by sorting them and thenchecking if theres a identical digit next to each other. 
bool check_dupe(string str) {
    sort(str.begin(), str.end());
    return adjacent_find(str.begin(), str.end()) == str.end();
}

//Makes sure that the input from the player is valid. 
string player_guess() {
    string guess;
    //loop until we get a valid answer.
    while(true) {
        cout << "Whats your guess?\n";
        cin >> guess;

        if (guess.length() == NUM_LEN &&
            guess.find_first_not_of(NUMBERS) == string::npos &&
            check_dupe(guess)){
            return guess;
        }
        cout << "invalid input\n";
    }
}

//calculates the score based on similarty by looping trough
void calculate_score(const string &answer, string guess, pair<int, int> &cb){
    cb.first = 0, cb.second = 0;
    for (int i = 0; i != NUM_LEN; i++){
    	//checks if the number at position i exsists in answer at the same position. otherwise it checks if it's in the answer att all.
        if (answer.find(guess[i]) == i){
            cb.second++;
        }
        else if (answer.find(guess[i]) != string::npos){
            cb.first++;
        }
    }
}

//grabs a random number from a vector list containing all possible gusses. 
string cpu_guess(vector<string> &list){
    if( list.empty() ) return "";
    return list[rand() % list.size()];
}

//Writes to high_score.txt (creates one if there isn't already)
//prints out: how many turns it took, what the answer was, and what mode it was.
void high_score(int turns, const string &number, string mode){
    ofstream output;
    output.open("High_score.txt", ios_base::app);
    output << "Turns: " << turns << "\nNumber: " << number << "\nMode " << mode << "\n============\n";
    output.close();
}

//generates a list with all NUM_LEN length digit combinations that don't repeat any digits. 
vector <string> generate_list(){
    vector<string> number_vector;
    for (int x = pow(10, NUM_LEN-2); x < pow(10, NUM_LEN)-1; x++)
    {
        //to make sure we include numbers that start with 0
        unsigned long long number_of_zeros = NUM_LEN - to_string(x).length();
        string temp = string(number_of_zeros, '0').append(to_string(x));
        if(check_dupe(temp)) {
            number_vector.push_back(temp);
        }
    }
    printf("%i - Number of possible guesses\n", number_vector.size());
    return number_vector;
}

//filters out invalid guesses.
//does it by first getting the # of bulls that the guess got.
//then doing a bulls and cow check vs the rest of the list.
void filter(const string &guess, pair<int, int> cows_bulls, vector<string> &list){
    auto it = list.begin();
    pair<int, int> filter_cb;
    while(it != list.end()){
        calculate_score(*it, guess, filter_cb);
        if (filter_cb.first < cows_bulls.first || filter_cb.second < cows_bulls.second || *it == guess){
            list.erase(it);
        }
        else{
            it++;
        }
    }
    printf("%i - Number of possible guesses remaining\n", list.size());
}

//player is the code breaker
void player_cpu(string &answer, string &guess, int &turn, pair<int, int> &cows_bulls){
    while (cows_bulls.second != NUM_LEN){
        turn++;
    guess = player_guess();
    printf("%s - Guess\n", guess.c_str());
    calculate_score(answer, guess, cows_bulls);
    printf("Turns: %i\nCows:  %i\nBulls: %i\n", turn, cows_bulls.first, cows_bulls.second);
    printf("========================================\n");
    }
    printf("Congratulations you win! the final answer was %s and it took you %i turns to solve.", answer.c_str(), turn);
    high_score(turn, answer, "Player vs CPU");
}

//the cpu is the code breaker
void cpu_player(string &guess, int &turn, pair<int, int> &cows_bulls){
    vector<string> list = generate_list();
    while(true){
        turn++;
        guess = cpu_guess(list);
        cows_bulls.second = cows_bulls.first = -1;
        printf("My guess is %s\nHow many cows and bulls did i get?\n", guess.c_str());
        while (cows_bulls.first < 0 || cows_bulls.first > NUM_LEN){
            printf("Cows: ");
            cin >> cows_bulls.first;
            if (cin.fail())
            {
                printf("Please type in a number between 0 and %i\n", NUM_LEN);
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cows_bulls.first = -1;
            }
        }
        while (cows_bulls.second < 0 || cows_bulls.second > NUM_LEN){
            printf("Bulls: ");
            cin >> cows_bulls.second;
            if (cin.fail())
            {
                printf("Please type in a number between 0 and %i\n", NUM_LEN);
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cows_bulls.second = -1;
            }
        }
        if (cows_bulls.second == NUM_LEN){
            break;
        }
        filter(guess, cows_bulls, list);
        if (list.empty()){
            printf("Ye, dirty cheat.\nYou Lose.");
            return;
        }
        printf("Turn:  %i\nCows:  %i\nBulls: %i\n", turn, cows_bulls.first, cows_bulls.second);
    }
    printf("I win!\nIt took me %i turns", turn);
    high_score(turn, guess, "CPU vs Player");
}

//the cpu plays vs it self. 
void cpu_cpu(string &answer, string &guess, int &turn,  pair<int, int> &cows_bulls){
    vector<string> list = generate_list();
    while (true) {
        turn++;
        guess = cpu_guess(list);
        printf("%s - Guess\n", guess.c_str());
        calculate_score(answer, guess, cows_bulls);
        if (cows_bulls.second == NUM_LEN){
            break;
        }
        printf("Turns: %i\nCows:  %i\nBulls: %i\n", turn, cows_bulls.first, cows_bulls.second);
        filter(guess, cows_bulls, list);
    }
    printf("solved in %i turns\nThe secret was %s", turn, answer.c_str());
    high_score(turn, answer, "CPU vs CPU");
}


int main() {
    int mode = 0;
    int turn = 0;
    string guess;
    pair <int, int> cows_bulls;
    string answer = generate_digits();
    while (mode < 1 || mode > 3){
        printf("Do you you want to be the code cracker - Press 1\n"
           "Let the computer be the code cracker - Press 2\n"
           "Or let the computer play vs itself? - Press 3\n");
        cin >> mode;
        if (cin.fail())
        {
            printf("Please type in a number between 1 and 3 to select your mode.\n");
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    if (mode == 1){
        player_cpu(answer, guess, turn, cows_bulls);
    }
    else if (mode == 2){
        cpu_player(guess, turn, cows_bulls);
    }
    else if (mode == 3){
        cpu_cpu(answer, guess, turn, cows_bulls);
    }
    return 0;
}