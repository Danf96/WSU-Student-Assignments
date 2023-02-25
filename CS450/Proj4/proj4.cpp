//Daniel Fuchs
//CS 450
//Written in C++
//Compiled during testing with g++
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <list>
#include <ctime>
using namespace std;

list<int> knapsack(int i, int j, vector<vector<int>>& mainVec, vector<tuple<string, int, int>>& itemVec);
void recursiveKnapSol(int n, int w, vector<vector<int>> &mainVec, vector<tuple<string, int, int>> &itemVec);

/*
    Based on Wikipedia pseudocode: https://en.wikipedia.org/wiki/Knapsack_problem#0-1_knapsack_problem
    Main difference is name, weight, and value are stored in a tuple
*/
int main(int argc, char* argv[]){
    clock_t start = clock();
    string line;
    int weightCapacity;
    vector<tuple<string, int, int>> itemVec;
    itemVec.reserve(129);
    itemVec.push_back(make_tuple(line, 0, 0));
    getline(cin, line);
    weightCapacity = stoi(line);
    while(getline(cin, line)){ //order in file is name, weight, value
        string name;
        string token;
        int weight;
        int value;
        int start = 0;
        int pos = line.find_first_of(';');
        name = line.substr(start, pos);
        start = pos + 1;
        pos = line.find(';', start);
        token = line.substr(start, pos - start);
        weight = stoi(token);
        start = pos + 1;
        token = line.substr(start, line.size() - start);
        value = stoi(token);
        itemVec.push_back(make_tuple(name, weight, value));
    }
    vector<vector<int>> mainVec(itemVec.size(),vector<int>(weightCapacity + 1, -1));
    recursiveKnapSol(itemVec.size()-1, weightCapacity, mainVec, itemVec);

    list<int> results = knapsack(itemVec.size()-1, weightCapacity, mainVec, itemVec);
    int finalWeight = 0;
    int finalValue = 0;
    for(auto i : results){
        cout << get<0>(itemVec[i]) << ", " << get<1>(itemVec[i]) << ", " << get<2>(itemVec[i]) << '\n';
        finalWeight += get<1>(itemVec[i]);
        finalValue += get<2>(itemVec[i]);
    }
    cout << "final weight: " << finalWeight << '\n';
    cout << "final value: " << finalValue << '\n';
    double time_taken_in_seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    double time_taken_in_microseconds = time_taken_in_seconds * 1000000.0;
    cout << "time taken in microseconds: " << time_taken_in_microseconds << '\n';

}

/*
    Based on Wikipedia pseudocode, rather than keeping multiple vectors, just using a vector of tuples instead
*/
void recursiveKnapSol(int i, int j, vector<vector<int>> &mainVec, vector<tuple<string, int, int>> &itemVec){
    if (i == 0 || j <= 0){
        mainVec[i][j] = 0;
        return;
    }
    if(mainVec[i-1][j] == -1){
        recursiveKnapSol(i-1, j, mainVec, itemVec);
    }
    if(get<1>(itemVec[i]) > j){
        mainVec[i][j] = mainVec[i-1][j];
    }
    else{
        if(mainVec[i-1][j - get<1>(itemVec[i])] == -1){
            recursiveKnapSol(i-1, j - get<1>(itemVec[i]), mainVec, itemVec);
        }
        mainVec[i][j] = max(mainVec[i-1][j], mainVec[i-1][j - get<1>(itemVec[i])] + get<2>(itemVec[i]));
    }
}

/*
    Gets subset of items to return as best item combination
*/
list<int> knapsack(int i, int j, vector<vector<int>>& mainVec, vector<tuple<string, int, int>>& itemVec){
    list<int> elements;
    if (i == 0){
        return elements;
    }
    if(mainVec[i][j] > mainVec[i-1][j] && mainVec[i-1][j] != -1){
        elements.push_back(i);
        list<int> returnedElements = knapsack(i-1, j - get<1>(itemVec[i]), mainVec, itemVec);
        elements.merge(returnedElements);
        return elements;
    }
    else{
        return knapsack(i-1, j, mainVec, itemVec);
    }
}
