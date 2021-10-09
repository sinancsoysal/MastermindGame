// MastermindPM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <string>
using namespace std;

vector<int> getRandomCode();

void createSet();

void combinationRecursive(int combinationLength, int position, vector<int>& current, vector<int>& elements);

string checkCode(vector<int> guess, vector<int> code);

void removeCode(vector<vector<int>>& set, vector<int> code);

void pruneCodes(vector<vector<int>>& set, vector<int> code, string currentResponse);

vector<vector<int>> minmax();

int getMaxScore(map<string, int> inputMap);

int getMinScore(map<vector<int>, int> inputMap);

vector<int> getNextGuess(vector<vector<int>> nextGuesses);

static const vector<int> validDigits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static const int NUMS = 9;
static const int CODE_LENGTH = 4;
static vector<vector<int>> combinations; //Master set of combinations 1234 to 9876
static vector<vector<int>> candidateSolutions;
static vector<vector<int>> nextGuesses;
static vector<int> code;
static vector<int> currentGuess;
static string responsePegs;
static bool won;
static int turn;

int main() {

    turn = 1;
    won = false;

    //Create the set of 3024 possible codes
    createSet();
    candidateSolutions.insert(candidateSolutions.end(), combinations.begin(), combinations.end());

    code = getRandomCode();
    currentGuess = { 1, 2, 3, 4 }; //1234

    cout << "Code: ";
    for (int i = 0; i < code.size(); ++i) {
        cout << code[i] << " ";
    }
    cout << endl;

    while (!won) {

        //Remove currentGuess from possible solutions
        removeCode(combinations, currentGuess);
        removeCode(candidateSolutions, currentGuess);

        //Play the guess to get a response of colored and white pegs
        responsePegs = checkCode(currentGuess, code);

        cout << "Turn: " << turn << endl;
        cout << "Guess: ";
        for (int i = 0; i < currentGuess.size(); ++i) {
            cout << currentGuess[i] << " ";
        }
        cout << "= " << responsePegs << endl;

        //If the response is four colored pegs, the game is won
        if (responsePegs == "4 0") {
            won = true;
            cout << "Game Won!" << endl;
            break;
        }

        //Remove from candidateSolutions,
        //any code that would not give the same response if it were the code
        pruneCodes(candidateSolutions, currentGuess, responsePegs);

        //Calculate Minmax scores
        nextGuesses = minmax();

        //Select next guess
        currentGuess = getNextGuess(nextGuesses);

        turn++;
    }//End while

    //getchar();
    return 0;
}

vector<int> getRandomCode() {

    vector<int> code;
    int max = NUMS;
    int min = 1;
    //int random;
    //Returns the number of seconds since the UNIX epoch for a seed
    srand((unsigned int)time(0));

    /*for (int i = 0; i < CODE_LENGTH; ++i) {
        random = min + (rand() % (max - min + 1));
        code.push_back(random);
    }*/

    code = (candidateSolutions.at(rand() % candidateSolutions.size()));

    return code;
}

void createSet() {
    for (int d1 = 1; d1 <= NUMS; d1++)
    {
        for (int d2 = 0; d2 <= NUMS; d2++)
        {
            for (int d3 = 0; d3 <= NUMS; d3++)
            {
                for (int d4 = 0; d4 <= NUMS; d4++)
                {
                    if (d1 != d2 && d1 != d3 && d1 != d4
                        && d2 != d3 && d2 != d4 && d3 != d4)
                    {
                        vector<int> comb;
                        comb.push_back(validDigits[d1]);
                        comb.push_back(validDigits[d2]);
                        comb.push_back(validDigits[d3]);
                        comb.push_back(validDigits[d4]);

                        combinations.push_back(comb);
                    }
                }
            }
        }
    }
}

string checkCode(vector<int> guess, vector<int> code) {

    string result;
    int plus = 0, minus = 0;

    for (int i = 0; i < CODE_LENGTH; ++i) {

        if (guess[i] == code[i]) {
            plus += 1;
            guess[i] *= -1;
            code[i] *= -1;
        }
        else {
            if (code[i] > 0) {

                vector<int>::iterator it = find(guess.begin(), guess.end(), code[i]);
                int64_t index;
                if (it != guess.end()) {
                    minus -= 1;
                    index = distance(guess.begin(), it);
                    guess[index] *= -1;
                }
            }
        }
    }

    result.append(to_string(plus) + " " + to_string(minus));

    return result;
}

void removeCode(vector<vector<int>>& set, vector<int> currentCode) {
    int64_t index;
    for (auto it = set.begin(); it != set.end(); it++) {
        index = distance(set.begin(), it);

        if (set[index] == currentCode) {
            set.erase(set.begin() + index);
            break;
        }
    }
}

void pruneCodes(vector<vector<int>>& set, vector<int> currentCode, string currentResponse) {

    int64_t index;
    vector<vector<int>>::iterator it = set.begin();

    while (it != set.end()) {
        index = distance(set.begin(), it);

        if (currentResponse != checkCode(currentCode, set[index])) {
            it = set.erase(set.begin() + index);
        }
        else {
            it++;
        }
    }
}

vector<vector<int>> minmax() {

    map<string, int> scoreCount;
    map<vector<int>, int> score;
    vector<vector<int>> nextGuesses;
    int max, min;

    for (int i = 0; i < combinations.size(); ++i) {

        for (int j = 0; j < candidateSolutions.size(); ++j) {

            string pegScore = checkCode(combinations[i], candidateSolutions[j]);
            if (scoreCount.count(pegScore) > 0) {
                scoreCount.at(pegScore)++;
            }
            else {
                scoreCount.emplace(pegScore, 1);
            }
        }

        max = getMaxScore(scoreCount);
        score.emplace(combinations[i], max);
        scoreCount.clear();
    }

    min = getMinScore(score);

    for (auto elem : score) {
        if (elem.second == min) {
            nextGuesses.push_back(elem.first);
        }
    }

    return nextGuesses;
}

int getMaxScore(map<string, int> inputMap) {

    int max = 0;
    for (auto elem : inputMap) {
        if (elem.second > max) {
            max = elem.second;
        }
    }

    return max;
}

int getMinScore(map<vector<int>, int> inputMap) {

    int min = numeric_limits<int>::max();
    for (auto elem : inputMap) {
        if (elem.second < min) {
            min = elem.second;
        }
    }

    return min;
}

vector<int> getNextGuess(vector<vector<int>> nextGuesses) {

    vector<int> nextGuess;

    for (int i = 0; i < nextGuesses.size(); ++i) {
        if (find(candidateSolutions.begin(), candidateSolutions.end(), nextGuesses[i]) != candidateSolutions.end()) {
            return nextGuesses[i];
        }
    }
    for (int j = 0; j < nextGuesses.size(); ++j) {
        if (find(combinations.begin(), combinations.end(), nextGuesses[j]) != combinations.end()) {
            return nextGuesses[j];
        }
    }

    return nextGuess;
}

//void createSet();
//string checkCode(vector<int> guess, vector<int> code);
//
//static const int NUMS = 9;
//static const int CODE_LENGTH = 4;
//static const vector<int> validDigits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//static vector<vector<int>> combinations;
//
//int main() {
//	/*createSet();*/
//
//	vector<int> code = { 6, 1, 9, 4 };
//	vector<int> guess = { 4, 1, 2, 6 };
//
//	cout << checkCode(guess, code) << endl;
//	return 0;
//}
//
//void createSet() {
//	for (int d1 = 1; d1 <= NUMS; d1++)
//	{
//		for (int d2 = 0; d2 <= NUMS; d2++)
//		{
//			for (int d3 = 0; d3 <= NUMS; d3++)
//			{
//				for (int d4 = 0; d4 <= NUMS; d4++)
//				{
//					if (d1 != d2 && d1 != d3 && d1 != d4
//						&& d2 != d3 && d2 != d4 && d3 != d4)
//					{
//						vector<int> comb;
//						comb.push_back(validDigits[d1]);
//						comb.push_back(validDigits[d2]);
//						comb.push_back(validDigits[d3]);
//						comb.push_back(validDigits[d4]);
//
//						combinations.push_back(comb);
//					}
//				}
//			}
//		}
//	}
//
//	for (int i = 0; i < combinations.size(); i++)
//	{
//		for (int j = 0; j < combinations[i].size(); j++)
//		{
//			cout << combinations[i][j] << " ";
//		}
//		cout << endl;
//	}
//}
//
//string checkCode(vector<int> guess, vector<int> code) {
//
//	string result;
//
//	int p = 0, m = 0;
//
//	for (int i = 0; i < CODE_LENGTH; i++) {
//
//		if (guess[i] == code[i]) {
//			p += 1;
//		}
//		else {
//			if (code[i] > 0) {
//				vector<int>::iterator it = find(guess.begin(), guess.end(), code[i]);
//				int index;
//				if (it != guess.end()) {
//					index = distance(guess.begin(), it);
//					m -= 1;
//				}
//			}
//		}
//	}
//
//	result.append(to_string(p) + " " + to_string(m));
//
//	return result;
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
