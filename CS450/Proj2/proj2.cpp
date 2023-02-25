//Daniel Fuchs
//CS 450
//Written in C++
//Compiled during testing with g++
//NOTE: expected input after compilation would be something like "./a.out custom < distributed_set.txt > test.txt"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

void standardSort(vector<vector<int>> &masterList);
void customSort(vector<vector<int>> &masterList);
void buildList(vector<vector<int>> &masterList);
void heapUp(vector<int> &heap, int index);
void heapInsert(vector<int> &heap, int value);
int parentIndex(int nodeIndex);
int removeAt(vector<int> &heap, int index);

/*
    Initial setup of the vectors we will use for reading from standard input then sorting
*/
int main(int argc, char* argv[]){
    vector<vector<int>> masterList(6);
    vector<int> breakDancing, underwaterApic, basketWeaving, xBasketWeaving, longsword, totalXP;
    masterList.push_back(breakDancing);
    masterList.push_back(underwaterApic);
    masterList.push_back(basketWeaving);
    masterList.push_back(xBasketWeaving);
    masterList.push_back(longsword);
    masterList.push_back(totalXP);
    buildList(masterList);
    if(string(argv[1]) == "standard"){
        standardSort(masterList);
    }else if(string(argv[1]) == "custom"){
        customSort(masterList);
    }
    return 0;
}

/*
    Gets a line from standard input, tokenizes it with >> behavior, then inserts tokens into respective vector
*/
void buildList(vector<vector<int>>& masterList){
    int currentToken, totalXP;
    string line;
    while(getline(cin, line)){
        totalXP = 0;
        int index = 0;
        istringstream stream_line(line);
        while(stream_line >> currentToken){
            totalXP += currentToken;
            masterList[index].push_back(currentToken);
            index++;
        }
        masterList[index].push_back(totalXP);
    }
}

/*
    Sorting function that just uses standard library sort and outputs the sorted vector line by line along with time taken
*/
void standardSort(vector<vector<int>> &masterList){
    vector<string> skillName = {"SKILL_BREAKDANCING", "SKILL_APICULTURE", "SKILL_BASKET", "SKILL_XBASKET", "SKILL_SWORD", "TOTAL_XP"};
    double time_taken_in_seconds, time_taken_in_microseconds, total_time;
    total_time = 0;
    for(int i = 0; i < 6; i++){
        clock_t start = clock();
        sort(masterList[i].begin(), masterList[i].end(), greater<int>());
        time_taken_in_seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
        time_taken_in_microseconds = time_taken_in_seconds * 1000000.0;
        total_time += time_taken_in_microseconds;
        cout << skillName[i] << "\n";
        for(auto num : masterList[i]){
            cout << num << "\n";
        }
        cout << "time taken: " << time_taken_in_microseconds << endl << endl;
    }
    cout << "total time taken: " << total_time;
}

/*
    Same as standardSort except a counting sort method is used for numbers between 0 and 10000 and any outliers are sorted with heap sort
*/
void customSort(vector<vector<int>> &masterList){
    vector<string> skillName = {"SKILL_BREAKDANCING", "SKILL_APICULTURE", "SKILL_BASKET", "SKILL_XBASKET", "SKILL_SWORD", "TOTAL_XP"};
    vector<int> outliers, sortedOutliers;
    outliers.reserve(500);
    sortedOutliers.reserve(500);
    double time_taken_in_seconds, time_taken_in_microseconds, total_time;
    total_time = 0;
    for(int i = 0; i < 6; i++){
        vector<int> countBuckets(10000, 0);
        clock_t start = clock();
        for(auto n : masterList[i]){
            if (n >= 0 && n < 10000){
                countBuckets[n]++;
            }
            else{
                heapInsert(outliers, n);
            }
        }
        while(outliers.size() > 0){
            sortedOutliers.push_back(removeAt(outliers, 0));
        }
        time_taken_in_seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
        time_taken_in_microseconds = time_taken_in_seconds * 1000000.0;
        total_time += time_taken_in_microseconds;
        cout << skillName[i] << "\n";
        for(auto num : sortedOutliers){
            cout << num << "\n";
        }
        for(int num = countBuckets.size() - 1; num >= 0; num--){
            while(countBuckets[num] > 0){
                cout << num << "\n";
                countBuckets[num]--;
            }
        }
        cout << "time taken: " << time_taken_in_microseconds << endl << endl;
        outliers.clear();
        sortedOutliers.clear();
        countBuckets.clear();
    }
    cout << "total time taken: " << total_time;
}

/*
    Standard heap insertion, parent index, left child index, heap up, heap down, and remove at functions based on Python code in lecture
*/
void heapInsert(vector<int> &heap, const int value){
    const int itemIndex = heap.size();
    heap.push_back(value);
    heapUp(heap, itemIndex);
}
int parentIndex(int nodeIndex){
    return((nodeIndex-1)/2);
}
int leftChildIndex(int nodeIndex){
    return (2 * nodeIndex) + 1;
}
void heapUp(vector<int> &heap, const int index){
    if (index == 0) return;
   int parent = parentIndex(index);
    if(heap[index] > heap[parent]){ 
        //swap the two
        swap(heap[index],heap[parent]);
        heapUp(heap, parent);
    }
}
void heapDown(vector<int> &heap, const int index){
    if (index >= heap.size()/2) return;
    int leftChild = leftChildIndex(index);
    int rightChild = leftChild + 1;
    if(leftChild >= heap.size()) return;
    if(rightChild < heap.size() && 
    heap[rightChild] > heap[leftChild] && 
    heap[rightChild] > heap[index]){
        swap(heap[index],heap[rightChild]);
        heapDown(heap, rightChild);
    }
    else if(heap[leftChild] > heap[index]){
        swap(heap[index], heap[leftChild]);
        heapDown(heap, leftChild);
    }
}
int removeAt(vector<int> &heap, const int index){
    int value = heap[index];
    int replacement_val = heap.back();
    heap.pop_back();
    if (heap.size() == 0) return value;
    heap[index] = replacement_val;
    if(replacement_val > value){
        heapUp(heap, index);
    }
    else heapDown(heap, index);
    return value;
}
