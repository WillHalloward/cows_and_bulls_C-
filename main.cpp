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
    cout << "secret number generated\n";
    return answer;
}

bool check_dupe(string str) {
    sort(str.begin(), str.end());
    return adjacent_find(str.begin(), str.end()) != str.end();
}

//currently not used, will be needed later.
string player_guess() {
    string guess;
    //loop until we get a valid answer.
    while(true) {
        cout << "Whats your guess?\n";
        getline(cin, guess);
        if (guess.length() != NUM_LEN ||
            guess.find_first_not_of(NUMBERS) != string::npos ||
            check_dupe(guess)){
            cout << "invalid input\n";
            continue;
        }
        return guess;
    }
}

bool calculate_score(const string &answer, string guess, pair<int, int> &cb){
    cb.first = 0, cb.second = 0;
    for (int i = 0; i != NUM_LEN; i++){
        if (answer.find(guess[i]) == i){
            cb.second++;
        }
        else if (answer.find(guess[i]) != string::npos){
            cb.first++;
        }
    }
    return cb.second == NUM_LEN;
}

string generate_guess(vector<string> &list){
    if( list.empty() ) return "";
    return list[rand() % list.size()];
}

void high_score(int turns, const string &number){
    ofstream output;
    output.open("High_score.txt", ios_base::app);
    output << "Turns: " << turns << "\nNumber: " << number << "\n============\n";
    output.close();
}

vector <string> generate_list(){
    vector<string> number_vector;
    for (int x = pow(10, NUM_LEN-2); x < pow(10, NUM_LEN)-1; x++)
    {
        //to make sure we include 0123, 0124, etc
        unsigned long long number_of_zeros = NUM_LEN - to_string(x).length();
        string temp = string(number_of_zeros, '0').append(to_string(x));
        if(!check_dupe(temp)) {
            number_vector.push_back(temp);
        }
    }
    cout << number_vector.size() << " - Number of possible guesses.\n";
    return number_vector;
}

void filter(const string &guess, pair<int, int> cb, vector<string> &list){
    auto it = list.begin();
    pair<int, int> filter_cb;
    while(it != list.end()){
        calculate_score(*it, guess, filter_cb);
        if (filter_cb.first < cb.first || filter_cb.second < cb.second || *it == guess){
            list.erase(it);
        }
        else{
            it++;
        }
    }
    cout << list.size() << " - Number of possible guesses remaining\n";
}

int main() {
    pair <int, int> cows_bulls;
    int turn = 0;
    string answer = generate_digits();
    vector<string> list = generate_list();
    cout << answer << " - The secret answer.\n"; //cheat to make it easier to test.
    while (!list.empty()) {
        turn++;
        //string guess = player_guess();
        string guess = generate_guess(list);
        cout << guess << " - Guess\n";
        if (calculate_score(answer, guess, cows_bulls)) {
            break;
        }
        cout << "Turn:  " << turn << "\nCows:  " << cows_bulls.first << "\nBulls: " << cows_bulls.second << endl;
        filter(guess, cows_bulls, list);
        cout << "========================================\n";
    }
    if (cows_bulls.second == NUM_LEN) {
        cout << "You win! the secret number was: " << answer << "\nIt took: " << turn << " turns\n";
        high_score(turn, answer);
    }
    else if (list.empty()){
        cout << "ye filthy cheat\n";
    }
    else{
        cout << "something went horribly wrong\n";
    }
    return 0;
}