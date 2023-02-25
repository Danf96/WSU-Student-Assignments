    #include <iostream>
    #include <vector>
    #include <limits>
    #include <algorithm>
    using namespace std;
    int cutRod(vector<int>& prices, vector<int>& solutions, int i){
        if (i == 0) return 0;
        if (solutions[i] != -1) return solutions[i];

        int m = 0;
        for(int j = 1; j <= i; j++){
            m = max(m, prices[j] + cutRod(prices, solutions, i - j));
        }
        solutions[i] = m;
        return m;

    }

    int main() {
    	vector<int> solutions(50, -1);
        solutions[0] = 0;
        vector<int> prices{0,1,5,9,10,17,17,20,24,30};
        vector<int> problems{0,1,2,3,4,5,6,7,8,9};

        for(auto length: problems){
            cout << "length = " << length << " price = " << cutRod(prices, solutions, length) << endl;
        }

    	return 0;
    }
 
