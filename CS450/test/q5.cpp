    #include <iostream>
    #include <vector>
    #include <cmath>
    using namespace std;
    int compareDistance(vector<int> & heap, const int index, const int parent){
        int indexDist, parentDist;
        if(heap[index] > 0){
            indexDist = abs(10 - heap[index]);
        }else{
            indexDist = abs(heap[index]) + 10;
        }
        if(heap[parent] > 0){
            parentDist = abs(10 - parent[index]);
        }else{
            parentDist = abs(parent[index]) + 10;
        }
        return (indexDist < parentDist);
    }

    int main() {
    	vector<int> test;
    	test.push_back(11);
    	test.push_back(-5)
    	cout << compareDistance(test, 1, 0) << endl;
    	return 0;
    }
