//Daniel Fuchs
//CS 450
//Written in C++
//Compiled during testing with g++ and clang++
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
#include <unordered_set>
#include <tuple>
#include <memory>
#include <cmath>
using namespace std;


struct Node{
    char tileType;
    uint8_t row;
    uint8_t col;
    float gCost;
    float hCost;
    Node* previous;
};

void heapInsert(vector<Node*> &heap, Node* value);
int parentIndex(int nodeIndex);
int leftChildIndex(int nodeIndex);
void heapUp(vector<Node*> &heap, const int index);
void heapDown(vector<Node*> &heap, const int index);
Node* removeAt(vector<Node*> &heap, const int index);
vector<Node*> getNeighbors(vector<vector<Node>>& gridVec, Node* source);
float euclidDist(Node* source, Node* dest);
float dist(Node* source, Node* dest);
float fCost(Node * node);
bool isAdjacent(Node* source, Node* dest);
float aStar(int row, int col, int targetRow, int targetCol, vector<vector<Node>> &gridVec);

int main(int argc, char* argv[]){
    clock_t start = clock();
    string line;
    int stride = 0;
    vector<vector<Node>> gridVec;
    gridVec.reserve(256);
    //auto joe = ifstream("./sample_input3.txt"); //makes for easier testing with VS code's debugger
    char token;
    uint8_t row = 0;
    tuple<uint8_t, uint8_t> startPos;
    tuple<uint8_t, uint8_t> destPos;
    while(getline(cin, line)){
        uint8_t col = 0;
        istringstream tokLine(line);
        vector<Node> rowVec;
        while(tokLine >> token){
            rowVec.push_back({token, row, col, numeric_limits<float>::max(), 0, nullptr});
            if(token == '0'){
                startPos = make_tuple(row, col);
            }
            else if(token == '2'){
                destPos = make_tuple(row, col);
            }
            col++;
        }
        gridVec.push_back(rowVec);
        row++;
    }

    auto totalCost = aStar(get<0>(startPos), get<1>(startPos), get<0>(destPos), get<1>(destPos), gridVec);
    for(auto i: gridVec){
        for(auto j: i){
            cout << j.tileType << ' ';
        }
        cout << '\n';
    }
    double time_taken_in_seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    double time_taken_in_microseconds = time_taken_in_seconds * 1000000.0;
    cout << "Total cost: " << totalCost << '\n';
    cout << "Time taken in microseconds: " << time_taken_in_microseconds << '\n';

}

bool operator==(const Node& lhs, const Node& rhs){
    return (lhs.col == rhs.col) && (lhs.row == rhs.row);
}


struct nodeHash{
    size_t operator()(const struct Node* n) const{
        uint16_t newId = (uint16_t)n->row << 8;
        newId = newId | (uint16_t)n->col;
        return(newId);
    }
};


float aStar(int row, int col, int targetRow, int targetCol, vector<vector<Node>> &gridVec){
    unordered_set<Node*, nodeHash> visited;
    vector<Node*> heap;
    vector<vector<bool>> openSet(256, vector<bool>(256, false));
    bool pathFound = false;
    Node* currentNode;
    gridVec[row][col].gCost = 0; //starting node is free
    heap.push_back(&gridVec[row][col]);
    while(heap.size() > 0){
        currentNode = removeAt(heap, 0);
        visited.insert(currentNode);
        if(currentNode->row == targetRow && currentNode->col == targetCol){
            pathFound = true;
            break;
        }
        auto neighbors = getNeighbors(gridVec, currentNode);
        for(auto neighbor: neighbors){
            if(visited.find(neighbor) == visited.end()){
                float potentialGCost = currentNode->gCost + dist(currentNode, neighbor);
                if(potentialGCost < neighbor->gCost){
                    neighbor->previous = currentNode;
                    neighbor->gCost = potentialGCost;
                    neighbor->hCost = euclidDist(neighbor, &gridVec[targetRow][targetCol]);
                    if(!openSet[neighbor->row][neighbor->col]){
                        heapInsert(heap, neighbor);
                    }
                }
            }
        }
    }
    if(pathFound == true){
        auto totalCost = currentNode->gCost;
        currentNode = currentNode->previous;
        while(currentNode->row != row || currentNode->col != col){
            currentNode->tileType = '*';
            currentNode = currentNode->previous;
        }
        return totalCost;
    }
    else{return 0;}

}
vector<Node*> getNeighbors(vector<vector<Node>>& gridVec, Node* source){
    vector<Node*> neighborList;
    bool goDown = false, goUp = false, goRight = false, goLeft = false;
    if(source->row < gridVec.size()-1){goDown = true;}
    if(source->row > 0){goUp = true;}
    if(source->col < gridVec[0].size()-1){goRight = true;}
    if(source->col > 0){goLeft = true;}
    if(goRight){neighborList.push_back(&gridVec[source->row][source->col + 1]);}
    if(goRight && goUp){neighborList.push_back(&gridVec[source->row - 1][source->col + 1]);}
    if(goUp){neighborList.push_back(&gridVec[source->row - 1][source->col]);}
    if(goUp && goLeft){neighborList.push_back(&gridVec[source->row - 1][source->col - 1]);}
    if(goLeft){neighborList.push_back(&gridVec[source->row][source->col - 1]);}
    if(goDown && goLeft){neighborList.push_back(&gridVec[source->row + 1][source->col - 1]);}
    if(goDown){neighborList.push_back(&gridVec[source->row + 1][source->col]);}
    if(goDown && goRight){neighborList.push_back(&gridVec[source->row + 1][source->col + 1]);}
    return neighborList;
    
}
float euclidDist(Node* source, Node* dest){
    return sqrt(pow((source->row - dest->row), 2)+ pow((source->col - dest->col), 2));
}
float dist(Node* source, Node* dest){
    if(dest->tileType == '3' || dest->tileType == '1'){return numeric_limits<float>::max();}
    if(dest->tileType == '2'){return 0.0f;}
    if(isAdjacent(source, dest)){
        if(dest->tileType == '.'){return 1.0f;}
        if(dest->tileType == ','){return 2.0f;}
        if(dest->tileType == 'o'){return 3.0f;}
        if(dest->tileType == '='){return 50.0f;}
        else return numeric_limits<float>::max(); //unknown type, can't pathfind
    }
    else{
        if(dest->tileType == '.'){return 1.0f * 1.5f;}
        if(dest->tileType == ','){return 2.0f * 1.5f;}
        if(dest->tileType == 'o'){return 3.0f * 1.5f;}
        if(dest->tileType == '='){return 50.0f * 1.5f;}
        else return numeric_limits<float>::max();;
    }
}
bool isAdjacent(Node* source, Node* dest){
    if(source->row == dest->row || source->col == dest->col){return true;}
    else{return false;}
}

/*
    Standard heap insertion, parent index, left child index, heap up, heap down, and remove at functions based on Python code in lecture
*/
void heapInsert(vector<Node*> &heap, Node* value){
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
void heapUp(vector<Node*> &heap, const int index){
    if (index == 0) return;
   int parent = parentIndex(index);
    if(fCost(heap[index]) < fCost(heap[parent])){ 
        //swap the two
        swap(heap[index],heap[parent]);
        heapUp(heap, parent);
    }
}
void heapDown(vector<Node*> &heap, const int index){
    if (index >= heap.size()/2) return;
    int leftChild = leftChildIndex(index);
    int rightChild = leftChild + 1;
    if(leftChild >= heap.size()) return;
    if(rightChild < heap.size() && 
    fCost(heap[rightChild]) < fCost(heap[leftChild]) && 
    fCost(heap[rightChild]) < fCost(heap[index])){
        swap(heap[index],heap[rightChild]);
        heapDown(heap, rightChild);
    }
    else if(fCost(heap[leftChild]) < fCost(heap[index])){
        swap(heap[index], heap[leftChild]);
        heapDown(heap, leftChild);
    }
}
Node* removeAt(vector<Node*> &heap, const int index){
    auto value = heap[index];
    auto replacement_val = heap.back();
    heap.pop_back();
    if (heap.size() == 0) return value;
    heap[index] = replacement_val;
    if(fCost(replacement_val) < fCost(value)){
        heapUp(heap, index);
    }
    else heapDown(heap, index);
    return value;
}
float fCost(Node * node){
    return node->gCost + node->hCost;
}
