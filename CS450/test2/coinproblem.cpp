    #include <iostream>
    #include <vector>
    #include <limits>
    using namespace std;
    int coinsNeeded(int cents, vector<int>& denoms, vector<int>& solutions){
        if(cents < 0) return -2;
        if(solutions[cents] != -1) return solutions[cents];
        int minimum = numeric_limits<int>::max();
        for(int i = denoms.size() - 1; i >= 0; i--){
            auto denom = denoms[i];
            auto maybe = coinsNeeded(cents - denom, denoms, solutions);
            if (maybe != -2 && maybe < minimum){
                minimum = maybe + 1;
                solutions[cents] = maybe + 1;
            }
            
        }
        if(minimum != -2 && minimum != numeric_limits<int>::max()){
                return minimum;
        }
        else{
            solutions[cents] = -2;
            return -2;
        }
    }

    int main() {
    	vector<int> solutions(50, -1);
        solutions[0] = 0;
        solutions[1] = -2;
        solutions[2] = 1;
        vector<int> denoms{2,7,10};
        vector<int> problems{4,5,10,11,12,24,30,31,34,35,37,40};
        for(auto cents: problems){
            cout << "c = " << cents << " count = " << coinsNeeded(cents, denoms, solutions) << endl;
        }
    	return 0;
    }
 
